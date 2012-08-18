#ifndef FSDOCUMENTMODEL_H
#define FSDOCUMENTMODEL_H

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QUndoStack>
#include <QMutex>

#include "fslayer.h"

typedef QStringList FSLayerPath;
typedef QList<QStringList> FSLayerPathList;

class QXmlStreamWriter;
class QXmlStreamReader;
class FSLayerEdit;
class FSDocumentCommand;
class FSZip;
class FSSurfacePainter;


class FSDocumentRootLayer : public FSLayer
{
public:
	FSDocumentRootLayer(FSDocumentModel *document) : FSLayer("root"), _document(document) {}
	FSDocumentRootLayer(const FSDocumentRootLayer &other) : FSLayer(other), _document(other._document) {}
	
	FSLayer *clone() const { return new FSDocumentRootLayer(*this); }
	Type type() const { return FSLayer::TypeGroup; }
	bool canHaveChildren() const { return true; }
	
	bool setProperty(const QVariant &, int) { return false; }
	
private:
	FSDocumentModel *_document;
};

class FSDocumentModel : public QAbstractItemModel
{
	Q_OBJECT
	friend class FSDocumentCommand;
	
public:
	
	FSDocumentModel(const QString &tempName, const QSize &size, const FSLayerList &layers, QObject *parent = 0);
	FSDocumentModel(const QString &tempName, const QSize &size, FSLayer *layer, QObject *parent = 0) :
		FSDocumentModel(tempName, size, mlListFromValue(layer), parent) {}
	
	static FSDocumentModel *open(const QString &filePath, QObject *parent = 0);
	bool saveAs(const QString &filePath);
	bool save() { return saveAs(filePath()); }
	
	QSize size() const { return _size; }
	int width() const { return _size.width(); }
	int height() const { return _size.height(); }
	bool isModified() const { return _modified; }
	bool isNew() const { return _filePath.isEmpty(); }
	QString filePath() const { return _filePath; }
	QString fileName() const { return _filePath.section('/', -1); }
	QString tempName() const { return _tempName; }
	int tileXCount() const { return _size.width() / MLSurface::TileSize + 1; }
	int tileYCount() const { return _size.height() / MLSurface::TileSize + 1; }
	QPointSet tileKeys() const { return _tileKeys; }
	bool canRedo() const { return _undoStack->canRedo(); }
	bool canUndo() const { return _undoStack->canUndo(); }
	
	void editLayer(const QModelIndex &index, FSLayerEdit *edit, const QString &description);
	void addLayer(FSLayer *layer, const QModelIndex &parent, int row, const QString &description) {
		QList<FSLayer *> layers;
		layers << layer;
		addLayers(layers, parent, row, description);
	}
	void addLayers(QList<FSLayer *> layers, const QModelIndex &parent, int row, const QString &description);
	void newLayer(FSLayer::Type type, const QModelIndex &parent, int row);
	void removeLayers(const QModelIndexList &indexes);
	void moveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row) { copyOrMoveLayers(indexes, parent, row, false); }
	void copyLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row) { copyOrMoveLayers(indexes, parent, row, true); }
	void renameLayer(const QModelIndex &index, const QString &newName);
	
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role, const QString &description);
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex index(int row, const QModelIndex &parent) const { return index(row, 0, parent); }
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &/*parent*/) const
		{ return 1; }
	Qt::DropActions supportedDropActions() const
		{ return Qt::MoveAction | Qt::CopyAction; }
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	
	FSLayer::Type layerType(const QModelIndex &index) const { return layerForIndex(index)->type(); }
	
	const FSLayer *layerForIndex(const QModelIndex &index) const;
	QModelIndex indexForLayer(const FSLayer *layer) const;
	
	const FSLayer *layerForPath(const FSLayerPath &path) const;
	FSLayerPath pathForLayer(const FSLayer *layer) const;
	
	QModelIndex indexForPath(const FSLayerPath &path) const { return indexForLayer(layerForPath(path)); }
	FSLayerPath pathForIndex(const QModelIndex &index) const { return pathForLayer(layerForIndex(index)); }
	
	QItemSelectionModel *selectionModel() const { return _selectionModel; }
	QModelIndex currentIndex() const { return _selectionModel->currentIndex(); }
	
	QUndoStack *undoStack() { return _undoStack; }
	
	/**
	  Returns the document's mutex.
	  Note that 
	 */
	QMutex *mutex() const { return &_mutex; }
	
signals:
	
	void modified();
	void tilesUpdated(const QPointSet &tileKeys);
	void modifiedChanged(bool modified);
	void filePathChanged(const QString &filePath);
	void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);
	void layerMetadataChanged(const QModelIndex &index);
	
	void undoTextChanged(const QString &name);
	void redoTextChangdd(const QString &name);
	
public slots:
	
	void undo() { _undoStack->undo(); }
	void redo() { _undoStack->redo(); }
	void updateDirtyThumbnails() { _rootLayer->updateDirtyThumbnailRecursive(size()); }
	
protected:
	
	void setModified(bool modified);
	void setFilePath(const QString &filePath);
	void emitDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) { emit dataChanged(topLeft, bottomRight); }
	void queueTileUpdate(const QPointSet &tileKeys) { _updatedTiles |= tileKeys; }
	
	FSLayer *rootLayer() { return _rootLayer.data(); }
	const FSLayer *rootLayer() const { return _rootLayer.data(); }
	
private slots:
	void undoStackIndexChanged(int index);
	
private:
	
	void copyOrMoveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row, bool copy);
	
	QString unduplicatedChildName(const QModelIndex &index, const QString &name) const;
	
	FSLayer *nonConstLayer(const FSLayer *layer) { return const_cast<FSLayer *>(layer); }
	
	static bool saveLayerRecursive(const FSLayer *parent, FSZip *archive, int *fileCount, QXmlStreamWriter *xmlWriter);
	static bool loadLayerRecursive(FSLayer *parent, FSZip *archive, QXmlStreamReader *xmlReader);
	
	QSize _size;
	QPointSet _tileKeys;
	QString _filePath;
	QString _tempName;	// like "untitled"
	bool _modified;
	QScopedPointer<FSDocumentRootLayer> _rootLayer;
	
	mutable QItemSelectionModel *_selectionModel;
	QUndoStack *_undoStack;
	
	QPointSet _updatedTiles;
	
	mutable QMutex _mutex;
};

#endif // FSDOCUMENTMODEL_H
