#pragma once

#include <QObject>
#include "layer.h"

class QUndoCommand;
class QItemSelectionModel;
class QItemSelection;

namespace PaintField {

class LayerEdit;
class Document;
class LayerItemModel;

class LayerScene : public QObject
{
	Q_OBJECT
public:
	LayerScene(const QList<LayerRef> &layers, Document *document);
	~LayerScene();
	
	void addLayers(const QList<LayerRef> &layers, const LayerConstRef &parent, int index, const QString &description);
	void removeLayers(const QList<LayerConstRef> &layers, const QString &description = QString());
	void moveLayers(const QList<LayerConstRef> &layers, const LayerConstRef &parent, int index);
	void copyLayers(const QList<LayerConstRef> &layers, const LayerConstRef &parent, int index);
	void mergeLayers(const LayerConstRef &parent, int index, int count);
	
	void editLayer(const LayerConstRef &layer, LayerEdit *edit, const QString &description);
	void setLayerProperty(const LayerConstRef &layer, const QVariant &data, int role, const QString &description = QString(), bool mergeOn = true);
	
	QList<LayerConstRef> topLevelLayers() const { return rootLayer()->children(); }
	
	LayerConstRef rootLayer() const;
	Document *document();
	
	LayerItemModel *itemModel();
	QItemSelectionModel *itemSelectionModel();
	
	LayerConstRef current() const;
	QList<LayerConstRef> selection() const;
	
	LayerConstRef layerForPath(const QList<int> &path);
	
	static QList<int> pathForLayer(const LayerConstRef &layer);
	
public slots:
	
	void abortThumbnailUpdate();
	void update();
	
	void setCurrent(const LayerConstRef &layer);
	void setSelection(const QList<LayerConstRef> &layers);
	
signals:
	
	void layerAboutToBeInserted(const LayerConstRef &parent, int index);
	void layerInserted(const LayerConstRef &parent, int index);
	
	void layerAboutToBeRemoved(const LayerConstRef &parent, int index);
	void layerRemoved(const LayerConstRef &parent, int index);
	
	void layerChanged(const LayerConstRef &layer);
	
	void tilesUpdated(const QPointSet &tileKeys);
	void thumbnailsUpdated(const QPointSet &updatedKeys);
	
	void currentChanged(const LayerConstRef &now, const LayerConstRef &old);
	void selectionChanged(const QList<LayerConstRef> &selected, const QList<LayerConstRef> &deselected);
	
	void currentLayerChanged();
	
protected:
	
	void enqueueTileUpdate(const QPointSet &keys);
	LayerRef mutableRootLayer();
	
	void pushCommand(QUndoCommand *command);
	
private slots:
	
	void updateDirtyThumbnails();
	
	void onCurrentIndexChanged(const QModelIndex &now, const QModelIndex &old);
	void onItemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void onLayerPropertyChanged(const LayerConstRef &layer);
	
private:
	
	friend class LayerSceneCommand;
	
	struct Data;
	Data *d;
};

} // namespace PaintField

