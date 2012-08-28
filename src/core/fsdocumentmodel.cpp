#include <QtGui>
#include "fszip.h"
#include "fslayeredit.h"
#include "fsrasterlayer.h"
#include "fsgrouplayer.h"
#include "fsrandomstring.h"
#include "mlblendmode.h"
#include "mlsurfacepainter.h"
#include "fsdocumentcommand.h"
#include "fslayerrenderer.h"

#include "fsdocumentmodel.h"

FSDocumentModel::FSDocumentModel(const QString &tempName, const QSize &size, const FSLayerList &layers,  QObject *parent) :
    QAbstractItemModel(parent),
    _size(size),
    _tempName(tempName),
    _modified(false),
	_rootLayer(new FSDocumentRootLayer(this) ),
    _selectionModel(new QItemSelectionModel(this, this) ),
    _undoStack(new QUndoStack(this) ),
	_skipNextUpdate(false)
{
	_tileKeys = MLSurface::keysForRect(QRect(QPoint(), size));
	
	connect(_undoStack, SIGNAL(redoTextChanged(QString)), this, SIGNAL(redoTextChangdd(QString)));
	connect(_undoStack, SIGNAL(undoTextChanged(QString)), this, SIGNAL(undoTextChanged(QString)));
	connect(_undoStack, SIGNAL(indexChanged(int)), this, SLOT(onUndoneOrRedone()));
	connect(_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)));
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(layerMetadataChanged(QModelIndex)));
	
	_rootLayer->insertChildren(0, layers);
	
	_rootLayer->updateThumbnailRecursive(size);
	
	_selectionModel->setCurrentIndex(index(0, QModelIndex()), QItemSelectionModel::Current);
}

FSDocumentModel *FSDocumentModel::open(const QString &filePath, QObject *parent)
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
	
	QXmlStreamAttributes attributes = xmlReader.attributes();
	
	if (attributes.value("version") != "1.0") {
		qWarning() << __PRETTY_FUNCTION__ << "incompatible file version";
		return 0;
	}
	
	FSGroupLayer group;
	QSize size;
	
	size.rwidth() = attributes.value("width").toString().toInt();
	size.rheight() = attributes.value("height").toString().toInt();
	
	while (xmlReader.readNextStartElement() ) {
		if (xmlReader.name() == "stack") {
			if (!loadLayerRecursive(&group, &archive, &xmlReader) ) {
				qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
				return 0;
			}
			// succeeded to open file
			
			FSDocumentModel *documentModel = new FSDocumentModel(QString(), size, group.takeChildren(), parent);
			documentModel->setFilePath(filePath);
			return documentModel;
		}
		xmlReader.skipCurrentElement();
	}
	
	qWarning() << __PRETTY_FUNCTION__ << "file is corrupt";
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
		xmlWriter->writeAttribute("visible", child->isVisible() ? "true" : "false");
		xmlWriter->writeAttribute("locked", child->isLocked() ? "true" : "false");
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

void FSDocumentModel::editLayer(const QModelIndex &index, FSLayerEdit *edit, const QString &description)
{
	QUndoCommand *command = new FSDocumentEditCommand(pathForIndex(index), edit, this);
	command->setText(description);
	
	_undoStack->push(command);
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
		return;
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

QString FSDocumentModel::unduplicatedChildName(const QModelIndex &index, const QString &name) const
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
	return setData(index, value, role, QString());
}

bool FSDocumentModel::setData(const QModelIndex &index, const QVariant &value, int role, const QString &description)
{
	QString text = description;
	
	if (!index.isValid() )
		return false;
	
	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		role = FSGlobal::RoleName;
		
		if (description.isNull())
			text = tr("Rename Layer");
		
		break;
	default:
		break;
	}
	
	if (data(index, role) == value)
		return true;
	
	if (role == FSGlobal::RoleName)
		renameLayer(index, value.toString());
	else
		editLayer(index, new FSLayerPropertyEdit(value, role), text);
	 
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

void FSDocumentModel::render(MLPainter *painter)
{
	FSLayerRenderer renderer;
	renderer.renderMultiple(painter, rootLayer(), tileKeys());
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

void FSDocumentModel::onUndoneOrRedone()
{
	if (_skipNextUpdate)
		_skipNextUpdate = false;
	else
		emit tilesUpdated(_updatedTiles);
	
	_updatedTiles.clear();
	
	emit modified();
	setModified(true);
}
