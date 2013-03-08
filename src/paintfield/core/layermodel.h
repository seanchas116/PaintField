#ifndef FSLAYERMODEL_H
#define FSLAYERMODEL_H

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QUndoCommand>

#include "grouplayer.h"
#include "layer.h"


namespace PaintField {

class Document;
class LayerModel;
class LayerEdit;
class LayerModelCommand;

class LayerPath : public QList<QString>
{
public:
	
	typedef QList<QString> super;
	
	LayerPath() : super() {}
	LayerPath(const LayerPath &other) : super(other) {}
	LayerPath(std::initializer_list<QString> args) : super(args) {}
	
	LayerPath parentPath() const
	{
		auto result = *this;
		result.removeLast();
		return result;
	}
	
	QString name() const { return last(); }
	
	LayerPath childPath(const QString &child) const
	{
		auto result = *this;
		result << child;
		return result;
	}
	
	LayerPath &operator=(const super &other)
	{
		super::operator=(other);
		return *this;
	}
};

typedef QList<LayerPath> LayerPathList;


class ModelRootLayer : public GroupLayer
{
public:
	ModelRootLayer(LayerModel *model) : GroupLayer("root"), _model(model) {}
	
	LayerModel *layerModel() { return _model; }
	
private:
	LayerModel *_model = 0;
};

class LayerModel : public QAbstractItemModel
{
	friend class LayerModelCommand;
	
	Q_OBJECT
public:
	
	explicit LayerModel(const LayerList &layers, Document *parent);
	~LayerModel();
	
	Document *document();
	
	void makeSkipNextUpdate();
	
	void editLayer(const QModelIndex &index, LayerEdit *edit, const QString &description);
	void addLayers(QList<Layer *> layers, const QModelIndex &parent, int row, const QString &description);
	void removeLayers(const QModelIndexList &indexes);
	void moveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row);
	void copyLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row);
	void renameLayer(const QModelIndex &index, const QString &newName);
	void mergeLayers(const QModelIndex &parent, int from, int to);
	
	//QMap<int, QVariant> itemData(const QModelIndex &index) const;
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
	
	bool checkIndexes(const QModelIndexList &indexes) const;
	
	bool checkIndex(const QModelIndex &index) const { return layerForIndex(index); }
	bool checkLayer(const Layer *layer) const { return rootLayer()->isAncestorOfSafe(layer); }
	
	const Layer *layerForIndex(const QModelIndex &index) const;
	QModelIndex indexForLayer(const Layer *layer) const;
	
	const Layer *layerForPath(const LayerPath &path) const;
	LayerPath pathForLayer(const Layer *layer) const;
	
	QModelIndex indexForPath(const LayerPath &path) const { return indexForLayer(layerForPath(path)); }
	LayerPath pathForIndex(const QModelIndex &index) const { return pathForLayer(layerForIndex(index)); }
	
	LayerConstList layersForIndexes(const QModelIndexList &indexes) const;
	LayerPathList pathsForIndexes(const QModelIndexList &indexes) const;
	
	QStringList childNames(const Layer *layer) const { return layer ? layer->childNames() : QStringList(); }
	QStringList childNames(const QModelIndex &index) const { return childNames(layerForIndex(index)); }
	QStringList childNames(const LayerPath &path) const { return childNames(layerForPath(path)); }
	
	Malachite::Surface render();
	
	const Layer *rootLayer() const;
	
signals:
	
	void tilesUpdated(const QPointSet &tileKeys);
	void layerMetadataChanged(const QModelIndex &index);
	void thumbnailsUpdated();
	
public slots:
	
	void startEditing();
	void update();
	void updateDirtyThumbnails();
	
protected:
	
	int normalizeItemRole(int role) const;
	
	void pushCommand(QUndoCommand *command);
	
	void emitDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) { emit dataChanged(topLeft, bottomRight); }
	void enqueueTileUpdate(const QPointSet &tileKeys);
	
private slots:
	
private:
	
	void copyOrMoveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row, bool copy);
	
	Layer *nonConstLayer(const Layer *layer) { return const_cast<Layer *>(layer); }
	
	struct Data;
	Data *d;
};

}

#endif // FSLAYERMODEL_H
