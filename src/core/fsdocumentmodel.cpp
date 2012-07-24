#include <QtGui>
#include "fszip.h"
#include "fslayeredit.h"
#include "fsrasterlayer.h"
#include "fsgrouplayer.h"
#include "fsrandomstring.h"
#include "mlblendmode.h"
#include "mlsurfacepainter.h"

#include "fsdocumentmodel.h"

class FSDocumentCommand : public QUndoCommand
{
public:
	FSDocumentCommand(FSDocumentModel *model, QUndoCommand *parent)
	    : QUndoCommand(parent),
	      _model(model)
	{
		Q_ASSERT(_model);
	}
	
protected:
	
	void beginInsertLayers(FSLayer *parent, int start, int end) { Q_ASSERT(parent); _model->beginInsertRows(_model->indexForLayer(parent), start, end); }
	void endInsertLayers() { _model->endInsertRows(); }
	void beginRemoveLayers(FSLayer *parent, int start, int end) { Q_ASSERT(parent); _model->beginRemoveRows(_model->indexForLayer(parent), start, end); }
	void endRemoveLayers() { _model->endRemoveRows(); }
	
	void emitDataChanged(FSLayer *layer)
	{
		Q_ASSERT(layer);
		QModelIndex index = _model->indexForLayer(layer);
		_model->emitDataChanged(index, index);
	}
	
	void queueTileUpdate(const QPointSet &tileKeys) { _model->queueTileUpdate(tileKeys); }
	
	FSDocumentModel *document() { return _model; }
	
	FSLayer *layerForPath(const FSLayerPath &path) { return _model->nonConstLayer(_model->layerForPath(path)); }
	
	FSLayerPath parentPath(const FSLayerPath &path)
	{
		Q_ASSERT(path.size());
		
		FSLayerPath result = path;
		result.removeLast();
		return result;
	}
	
private:
	FSDocumentModel *_model;
};

class FSDocumentEditCommand : public FSDocumentCommand
{
public:
	FSDocumentEditCommand(const FSLayerPath &path, FSLayerEdit *edit, FSDocumentModel *document, QUndoCommand *parent = 0) :
		FSDocumentCommand(document, parent),
		_path(path),
		_edit(edit)
	{
		const FSLayer *layer = document->layerForPath(path);
		Q_ASSERT(layer);
		Q_ASSERT(edit);
		setText(edit->name());
		edit->saveUndoState(layer);
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	QScopedPointer<FSLayerEdit> _edit;
};

void FSDocumentEditCommand::redo()
{
	FSLayer *layer = layerForPath(_path);
	_edit->redo(layer);
	layer->updateThumbnail(document()->size());
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		queueTileUpdate(document()->tileKeys());
	else
		queueTileUpdate(_edit->modifiedKeys());
}

void FSDocumentEditCommand::undo()
{
	FSLayer *layer = layerForPath(_path);
	_edit->undo(layer);
	layer->updateThumbnail(document()->size());
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		queueTileUpdate(document()->tileKeys());
	else
		queueTileUpdate(_edit->modifiedKeys());
}

class FSDocumentAddCommand : public FSDocumentCommand
{
public:
	FSDocumentAddCommand(const FSLayer *layer, const FSLayerPath &newParentPath, int newRow, FSDocumentModel *document, QUndoCommand *parent = 0) : 
		FSDocumentCommand(document, parent),
		_newParentPath(newParentPath),
		_newRow(newRow)
	{
		Q_ASSERT(layer);
		_layer.reset(layer->clone());
	}
	
	void redo();
	void undo();
	
private:
	QScopedPointer<FSLayer> _layer;
	FSLayerPath _newParentPath;
	int _newRow;
};

void FSDocumentAddCommand::redo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	FSLayer *newLayer = _layer->clone();
	
	beginInsertLayers(parent, _newRow, _newRow);
	parent->insertChild(_newRow, newLayer);
	newLayer->updateThumbnail(document()->size());
	endInsertLayers();
	
	queueTileUpdate(_layer->surface().keys());
}

void FSDocumentAddCommand::undo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	
	beginRemoveLayers(parent, _newRow, _newRow);
	delete parent->takeChild(_newRow);
	endRemoveLayers();
	
	queueTileUpdate(_layer->tileKeysRecursive());
}


class FSDocumentRemoveCommand : public FSDocumentCommand
{
public:
	FSDocumentRemoveCommand(const FSLayerPath &path, FSDocumentModel *document, QUndoCommand *parent) :
		FSDocumentCommand(document, parent),
		_path(path)
	{}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	QScopedPointer<FSLayer> _layer;
	int _row;
};

void FSDocumentRemoveCommand::redo()
{
	_layer.reset(layerForPath(_path));
	_row = _layer->row();
	FSLayer *parent = _layer->parent();
	
	beginRemoveLayers(parent, _row, _row);
	parent->takeChild(_row);
	endRemoveLayers();
	
	queueTileUpdate(_layer->tileKeysRecursive());
}

void FSDocumentRemoveCommand::undo()
{
	FSLayerPath parentPath = _path;
	parentPath.removeLast();
	FSLayer *parent = layerForPath(parentPath);
	
	beginInsertLayers(parent, _row, _row);
	parent->insertChild(_row, _layer.take());
	endInsertLayers();
	
	queueTileUpdate(_layer->tileKeysRecursive());
}


class FSDocumentMoveCommand : public FSDocumentCommand
{
public:
	FSDocumentMoveCommand(const FSLayerPath &oldPath, const FSLayerPath &newPath, int newRow, FSDocumentModel *document, QUndoCommand *parent) :
		FSDocumentCommand(document, parent),
		_newRow(newRow)
	{
		FSLayer *layer = layerForPath(oldPath);
		_oldRow = layer->row();
		
		_oldParentPath = oldPath;
		_oldParentPath.removeLast();
		_oldName = oldPath.last();
		
		_newParentPath = newPath;
		_newParentPath.removeLast();
		_newName = newPath.last();
		
		if (_newParentPath == _oldParentPath && _newRow > _oldRow)
			_newRow--;
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _newParentPath, _oldParentPath;
	int _newRow, _oldRow;
	QString _newName, _oldName;
};

void FSDocumentMoveCommand::redo()
{
	//if (_newParentPath == _oldParentPath && _newRow == _oldRow)
	//	return;
	
	FSLayer *oldParent = layerForPath(_oldParentPath);
	FSLayer *newParent = layerForPath(_newParentPath);
	
	beginRemoveLayers(oldParent, _oldRow, _oldRow);
	FSLayer *layer = oldParent->takeChild(_oldRow);
	endRemoveLayers();
	
	layer->setName(_newName);
	
	beginInsertLayers(newParent, _newRow, _newRow);
	newParent->insertChild(_newRow, layer);
	endInsertLayers();
	
	queueTileUpdate(layer->tileKeysRecursive());
}

void FSDocumentMoveCommand::undo()
{
	if (_newParentPath == _oldParentPath && _newRow == _oldRow)
		return;
	
	FSLayer *oldParent = layerForPath(_oldParentPath);
	FSLayer *newParent = layerForPath(_newParentPath);
	
	beginRemoveLayers(newParent, _newRow, _newRow);
	FSLayer *layer = newParent->takeChild(_newRow);
	endRemoveLayers();
	
	layer->setName(_oldName);
	
	beginInsertLayers(oldParent, _oldRow, _oldRow);
	oldParent->insertChild(_oldRow, layer);
	endInsertLayers();
	
	queueTileUpdate(layer->tileKeysRecursive());
}


class FSDocumentCopyCommand : public FSDocumentCommand
{
public:
	FSDocumentCopyCommand(const FSLayerPath &path, const FSLayerPath &newPath, int newRow, FSDocumentModel *document, QUndoCommand *parent = 0) :
		FSDocumentCommand(document, parent),
		_path(path),
		_newRow(newRow)
	{
		_newParentPath = newPath;
		_newParentPath.removeLast();
		_newName = newPath.last();
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	FSLayerPath _newParentPath;
	QString _newName;
	int _newRow;
};

void FSDocumentCopyCommand::redo()
{
	FSLayer *layer = layerForPath(_path);
	FSLayer *parent = layerForPath(_newParentPath);
	FSLayer *clone = layer->cloneRecursive();
	clone->setName(_newName);
	beginInsertLayers(parent, _newRow, _newRow);
	parent->insertChild(_newRow, clone);
	endInsertLayers();
	queueTileUpdate(clone->tileKeysRecursive());
}

void FSDocumentCopyCommand::undo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	beginRemoveLayers(parent, _newRow, _newRow);
	FSLayer *layer = parent->takeChild(_newRow);
	endRemoveLayers();
	queueTileUpdate(layer->tileKeysRecursive());
	delete layer;
}



FSDocumentModel::FSDocumentModel(const QString &tempName, const QSize &size, QObject *parent) :
    QAbstractItemModel(parent),
    _size(size),
    _tempName(tempName),
    _modified(false),
	_rootLayer(new FSDocumentRootLayer(this) ),
    _selectionModel(new QItemSelectionModel(this, this) ),
    _undoStack(new QUndoStack(this) )
{
	_tileKeys = MLSurface::keysForRect(QRect(QPoint(), size));
	
	_rootLayer->insertChild(0, new FSRasterLayer(tr("Untitled")));
	_rootLayer->updateThumbnailRecursive(size);
	
	connect(_undoStack, SIGNAL(redoTextChanged(QString)), this, SIGNAL(redoTextChangdd(QString)));
	connect(_undoStack, SIGNAL(undoTextChanged(QString)), this, SIGNAL(undoTextChanged(QString)));
	connect(_undoStack, SIGNAL(indexChanged(int)), this, SLOT(undoStackIndexChanged(int)));
	connect(_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)));
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(layerMetadataChanged(QModelIndex)));
}

FSDocumentModel *FSDocumentModel:: open(const QString &filePath, QObject *parent)
{
	if (filePath.isEmpty()) {
		qWarning() << __PRETTY_FUNCTION__ << "file path is unknown";
		return 0;
	}
	
	FSZip archive;
	if (!archive.open(filePath)) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to open file";
		return 0;
	}
	
	QByteArray headerXml = archive.readFile("header.xml");
	if (headerXml.isNull()) {
		qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
		return 0;
	}
	
	QXmlStreamReader xmlReader(headerXml);
	
	xmlReader.readNextStartElement();
	
	if (xmlReader.name() != "document") {
		qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
		return 0;
	}
	
	FSDocumentModel *documentModel = new FSDocumentModel(QString(), QSize(), parent);	// create a new document
	documentModel->_filePath = filePath;
	
	QXmlStreamAttributes attributes = xmlReader.attributes();
	documentModel->_size.rwidth() = attributes.value("width").toString().toInt();
	documentModel->_size.rheight() = attributes.value("height").toString().toInt();
	
	if (attributes.value("version") != "1.0") {
		qWarning() << __PRETTY_FUNCTION__ << "incompatible file version";
		delete documentModel;
		return 0;
	}
	
	while (xmlReader.readNextStartElement() ) {
		if (xmlReader.name() == "stack") {
			if (!loadLayerRecursive(documentModel->rootLayer(), &archive, &xmlReader) ) {
				qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
				delete documentModel;
				return 0;
			}
			// succeeded to open file
			documentModel->rootLayer()->updateThumbnailRecursive(documentModel->size());
			return documentModel;
		}
		xmlReader.skipCurrentElement();
	}
	
	qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
	delete documentModel;
	return 0;
}

bool FSDocumentModel::saveAs(const QString &filePath)
{
	if (filePath.isEmpty()) {
		qWarning() << __PRETTY_FUNCTION__ << "filePath is empty";
		return false;
	}
	
	QByteArray headerXml;
	QXmlStreamWriter xmlWriter(&headerXml);
	
	xmlWriter.setAutoFormatting(true);
	
	QString tempFilePath = fsCreateTemporaryFilePath();
	FSZip archive;
	if (!archive.open(tempFilePath, ZIP_CREATE)) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to save file";
		return false;
	}
	
	int fileCount = 0;
	
	xmlWriter.writeStartDocument();
	
	xmlWriter.writeStartElement("document");
	xmlWriter.writeAttribute("width", QString::number(width() ) );
	xmlWriter.writeAttribute("height", QString::number(height() ) );
	xmlWriter.writeAttribute("version", "1.0");
	
	xmlWriter.writeStartElement("stack");
	if (!saveLayerRecursive(rootLayer(), &archive, &fileCount, &xmlWriter)) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to save file";
		return false;
	}
	
	xmlWriter.writeEndElement();	// stack
	xmlWriter.writeEndElement();	// document
	xmlWriter.writeEndDocument();
	
	if (!archive.writeFile("header.xml", headerXml)) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to save file";
		return false;
	}
	archive.close();
	
	if (QFile::exists(filePath)) {
		QFile::remove(filePath);
	}
	
	if (!QFile::copy(tempFilePath, filePath) ) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to copy file to the specified place";
		return false;
	}
	
	// succeeded to save file
	setFilePath(filePath);
	setModified(false);
	return true;
}

bool FSDocumentModel::saveLayerRecursive(const FSLayer *parent, FSZip *archive, int *fileCount, QXmlStreamWriter *xmlWriter)
{
	Q_ASSERT(parent);
	Q_ASSERT(archive);
	Q_ASSERT(fileCount);
	Q_ASSERT(xmlWriter);
	
	if (parent->type() != FSLayer::TypeGroup)
		return false;
	
	foreach (const FSLayer *child, parent->children() ) {
		xmlWriter->writeStartElement("layer");
		
		switch (child->type()) {
		case FSLayer::TypeGroup:
			xmlWriter->writeAttribute("type", "group");
			break;
		case FSLayer::TypeRaster:
			xmlWriter->writeAttribute("type", "raster");
			break;
		default:
			continue;
		}
		
		xmlWriter->writeAttribute("name", child->name() );
		xmlWriter->writeAttribute("visible", child->visible() ? "true" : "false");
		xmlWriter->writeAttribute("locked", child->locked() ? "true" : "false");
		xmlWriter->writeAttribute("opacity", QString::number(child->opacity() ) );
		xmlWriter->writeAttribute("blend_mode", child->blendMode().name() );
		
		if (child->type() == FSLayer::TypeGroup) {
			if (!saveLayerRecursive(child, archive, fileCount, xmlWriter) ) {
				return false;
			}
		} else if (child->type() == FSLayer::TypeRaster && !child->surface().isNull()) {
			QString fileName = "data/" + QString::number(*fileCount) + ".surface";
			
			QByteArray byteArray;
			QBuffer buffer(&byteArray);
			
			buffer.open(QIODevice::WriteOnly);
			child->surface().save(&buffer);
			buffer.close();
			
			archive->writeFile(fileName, byteArray);
			
			*fileCount += 1;
			
			xmlWriter->writeAttribute("source", fileName);
		}
		xmlWriter->writeEndElement();
	}
	
	return true;
}

bool FSDocumentModel::loadLayerRecursive(FSLayer *parent, FSZip *archive, QXmlStreamReader *xmlReader)
{
	Q_ASSERT(parent);
	Q_ASSERT(archive);
	Q_ASSERT(xmlReader);
	
	while (xmlReader->readNextStartElement()) {
		
		// qDebug() << __PRETTY_FUNCTION__ << "starting parsing";
		
		if (xmlReader->name() != "layer") {
			xmlReader->skipCurrentElement();
			continue;
		}
		
		QXmlStreamAttributes attributes = xmlReader->attributes();
		
		if (attributes.value("type") == "group") {
			FSGroupLayer *group = new FSGroupLayer();
			
			group->setName(attributes.value("name").toString() );
			group->setVisible(attributes.value("visible") == "true" ? true : false);
			group->setLocked(attributes.value("locked") == "true" ? true : false);
			group->setOpacity(attributes.value("opacity").toString().toDouble() );
			group->setBlendMode(MLBlendMode(attributes.value("blend_mode").toString() ) );
			
			parent->appendChild(group);
			
			if (!loadLayerRecursive(group, archive, xmlReader) ) {
				return false;
			}
		} else if (attributes.value("type") == "raster") {
			FSRasterLayer *raster = new FSRasterLayer();
			
			raster->setName(attributes.value("name").toString() );
			raster->setVisible(attributes.value("visible") == "true" ? true : false);
			raster->setLocked(attributes.value("locked") == "true" ? true : false);
			raster->setOpacity(attributes.value("opacity").toString().toDouble() );
			raster->setBlendMode(MLBlendMode(attributes.value("blend_mode").toString() ) );
			
			if (attributes.hasAttribute("source")) {
				QByteArray byteArray = archive->readFile(attributes.value("source").toString());
				if (byteArray.isNull())
					return false;
				
				QBuffer buffer(&byteArray);
				buffer.open(QIODevice::ReadOnly);
				raster->setSurface(MLSurface::loaded(&buffer));
			}
			
			parent->appendChild(raster);
			
			xmlReader->skipCurrentElement();
		}
	}
	return true;
}

void FSDocumentModel::editLayer(const QModelIndex &index, FSLayerEdit *edit)
{
	_undoStack->push(new FSDocumentEditCommand(pathForIndex(index), edit, this));
}

void FSDocumentModel::addLayers(QList<FSLayer *> layers, const QModelIndex &parent, int row, const QString &description)
{
	QUndoCommand *command = new QUndoCommand(description);
	
	for (int i = 0; i < layers.size(); ++i) {
		new FSDocumentAddCommand(layers.at(i), pathForIndex(parent), row + i, this, command);
	}
	
	_undoStack->push(command);
}

void FSDocumentModel::newLayer(FSLayer::Type type, const QModelIndex &parent, int row)
{
	FSLayer *layer;
	QString layerName, editName;
	
	switch(type) {
	case FSLayer::TypeGroup:
		layer = new FSGroupLayer();
		layerName = tr("Untitled Group");
		editName = tr("New Group");
		break;
	case FSLayer::TypeRaster:
		layer = new FSRasterLayer();
		layerName = tr("Untitled Layer");
		editName = tr("New Layer");
		break;
	default:
		Q_ASSERT(0);
	}
	
	const FSLayer *parentLayer = layerForIndex(parent);
	layer->setName(parentLayer->unduplicatedChildName(layerName));
	
	QUndoCommand *command = new FSDocumentAddCommand(layer, pathForIndex(parent), row, this);
	command->setText(editName);
	_undoStack->push(command);
	
	delete layer;
}

void FSDocumentModel::removeLayers(const QModelIndexList &indexes)
{
	QUndoCommand *command = new QUndoCommand(tr("Remove Layers"));
	
	foreach (const QModelIndex &index, indexes) {
		if (!index.isValid()) {
			qWarning() << __PRETTY_FUNCTION__ << ": indexes contains an invalid one.";
			continue;
		}
		new FSDocumentRemoveCommand(pathForIndex(index), this, command);
	}
	
	_undoStack->push(command);
}

void FSDocumentModel::copyOrMoveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row, bool copy)
{
	QUndoCommand *command = new QUndoCommand(copy ? tr("Copy Layers") : tr("Move Layers"));
	
	FSLayerPath newParentPath = pathForIndex(parent);
	
	for (int i = 0; i < indexes.size(); ++i)
	{
		QModelIndex index = indexes.at(i);
		
		if (!index.isValid())
		{
			qWarning() << __PRETTY_FUNCTION__ << ": indexes contains an invalid one.";
			continue;
		}
		
		FSLayerPath oldPath, newPath;
		oldPath = pathForIndex(index);
		
		bool rowChange = false;
		
		if (!copy)
		{
			FSLayerPath oldParentPath = oldPath;
			oldParentPath.removeLast();
			
			if (oldParentPath == newParentPath)
			{
				// move in same directory, name duplication check not needed
				newPath = oldPath;
				rowChange = true;
			}
		}
		
		if (!rowChange)
		{
			QString undupedName = unduplicatedChildName(parent, oldPath.last());
			newPath = newParentPath;
			newPath << undupedName;
		}
		
		if (copy)
			new FSDocumentCopyCommand(oldPath, newPath, row + i, this, command);
		else
			new FSDocumentMoveCommand(oldPath, newPath, row + i, this, command);
	}
	
	_undoStack->push(command);
}

void FSDocumentModel::renameLayer(const QModelIndex &index, const QString &newName)
{
	QString unduplicatedName = unduplicatedChildName(index.parent(), newName);
	
	FSLayerPath oldPath = pathForIndex(index);
	FSLayerPath newPath = oldPath;
	newPath.last() = unduplicatedName;
	
	QUndoCommand *command = new FSDocumentMoveCommand(oldPath, newPath, index.row(), this, 0);
	command->setText(tr("Rename Layer"));
	
	_undoStack->push(command);
}

QString FSDocumentModel::unduplicatedChildName(const QModelIndex &index, const QString &name)
{
	const FSLayer *layer = layerForIndex(index);
	if (!layer) return QString();
	
	return layer->unduplicatedChildName(name);
}

QVariant FSDocumentModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() )
		return QVariant();
	
	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		role = FSGlobal::RoleName;
		break;
	case Qt::DecorationRole:
		role = FSGlobal::RoleThumbnail;
		break;
	default:
		break;
	}
	
	const FSLayer *layer = layerForIndex(index);
	return layer->property(role);
}

bool FSDocumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() )
		return false;
	
	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		role = FSGlobal::RoleName;
		break;
	default:
		break;
	}
	
	if (data(index, role) == value)
		return true;
	
	if (role == FSGlobal::RoleName)
		renameLayer(index, value.toString());
	else
		editLayer(index, new FSLayerPropertyEdit(value, role) );
	
	return true;
}

Qt::ItemFlags FSDocumentModel::flags(const QModelIndex &index) const
{
	if (layerForIndex(index)->type() == FSLayer::TypeGroup)
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
}

QModelIndex FSDocumentModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) )
		return QModelIndex();	// ルート
	
	const FSLayer *childItem = layerForIndex(parent)->child(row);
	return createIndex(row, column, const_cast<FSLayer *>(childItem) );
}

QModelIndex FSDocumentModel::parent(const QModelIndex &child) const
{
	if (!child.isValid() )
		return QModelIndex();
	
	const FSLayer *parentItem = layerForIndex(child)->parent();
	
	if (parentItem == rootLayer())
		return QModelIndex();
	
	return createIndex(parentItem->row(), 0, const_cast<FSLayer *>(parentItem) );
}

int FSDocumentModel::rowCount(const QModelIndex &parent) const
{
	return layerForIndex(parent)->childCount();
}

QStringList FSDocumentModel::mimeTypes() const
{
	QStringList types;
	types << "application/x-freestyle-internal-layer-ref";
	return types;
}

QMimeData *FSDocumentModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData;
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	
	foreach (const QModelIndex &index, indexes) {
		stream << reinterpret_cast<quint64>(index.internalPointer() );
	}
	
	mimeData->setData("application/x-freestyle-internal-layer-ref", encodedData);
	
	return mimeData;
}

bool FSDocumentModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column);
	
	if (action == Qt::IgnoreAction)
		return true;
	if (!data->hasFormat("application/x-freestyle-internal-layer-ref") )	// 非対応のMimeType
		return false;
	
	if (row == -1)
		row = rowCount(parent);
	
	QByteArray encodedData = data->data("application/x-freestyle-internal-layer-ref");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QModelIndexList indexes;
	
	while (!stream.atEnd() ) {
		quint64 p;
		stream >> p;
		indexes << indexForLayer(reinterpret_cast<FSLayer *>(p));
	}
	
	switch (action) {
	case Qt::CopyAction:
		copyLayers(indexes, parent, row);
		break;
	case Qt::MoveAction:
		moveLayers(indexes, parent, row);
		break;
	default:
		return false;
	}
	return true;
}

const FSLayer *FSDocumentModel::layerForIndex(const QModelIndex &index) const
{
	const FSLayer *layer;
	if (index.isValid()) {
		layer = static_cast<FSLayer *>(index.internalPointer());
		if (layer->root() != rootLayer())
		{
			qWarning() << __PRETTY_FUNCTION__ << ": strange index";
			return 0;
		}
	} else {
		layer = rootLayer();
	}
	return layer;
}

QModelIndex FSDocumentModel::indexForLayer(const FSLayer *layer) const
{
	if (layer->root() != rootLayer())
	{
		qWarning() << __PRETTY_FUNCTION__ << ": strange layer";
		return QModelIndex();
	}
	return layer != rootLayer() ? createIndex(layer->row(), 0, const_cast<FSLayer *>(layer) ) : QModelIndex();
}

const FSLayer *FSDocumentModel::layerForPath(const FSLayerPath &path) const
{
	const FSLayer *layer = rootLayer();
	foreach (const QString &name, path) {
		layer = layer->child(name);
		if (!layer) {
			return 0;
		}
	}
	return layer;
}

FSLayerPath FSDocumentModel::pathForLayer(const FSLayer *layer) const
{
	if (layer->root() != rootLayer())
	{
		qWarning() << __PRETTY_FUNCTION__ << ": strange layer";
		return FSLayerPath();
	}
	
	FSLayerPath path;
	while (layer != rootLayer()) {
		path.prepend(layer->name());
		layer = layer->parent();
	}
	return path;
}

void FSDocumentModel::setModified(bool modified)
{
	if (_modified == modified)
		return;
	_modified = modified;
	emit modifiedChanged(modified);
}

void FSDocumentModel::setFilePath(const QString &filePath)
{
	if (_filePath == filePath)
		return;
	_filePath = filePath;
	emit filePathChanged(filePath);
}

void FSDocumentModel::undoStackIndexChanged(int /*index*/)
{
	emit tilesUpdated(_updatedTiles);
	_updatedTiles.clear();
	
	emit modified();
	setModified(true);
}
