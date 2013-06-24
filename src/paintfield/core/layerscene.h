#ifndef PAINTFIELD_LAYERSCENE_H
#define PAINTFIELD_LAYERSCENE_H

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
	LayerScene(const QList<LayerPtr> &layers, Document *document);
	~LayerScene();
	
	void addLayers(const QList<LayerPtr> &layers, const LayerConstPtr &parent, int index, const QString &description);
	void removeLayers(const QList<LayerConstPtr> &layers, const QString &description = QString());
	void moveLayers(const QList<LayerConstPtr> &layers, const LayerConstPtr &parent, int index);
	void copyLayers(const QList<LayerConstPtr> &layers, const LayerConstPtr &parent, int index);
	void mergeLayers(const LayerConstPtr &parent, int index, int count);
	
	void editLayer(const LayerConstPtr &layer, LayerEdit *edit, const QString &description);
	void setLayerProperty(const LayerConstPtr &layer, const QVariant &data, int role, const QString &description = QString(), bool mergeOn = true);
	
	LayerConstPtr rootLayer() const;
	Document *document();
	
	LayerItemModel *itemModel();
	QItemSelectionModel *itemSelectionModel();
	
	LayerConstPtr current() const;
	QList<LayerConstPtr> selection() const;
	
	LayerConstPtr layerForPath(const QList<int> &path);
	
	static QList<int> pathForLayer(const LayerConstPtr &layer);
	
public slots:
	
	void abortThumbnailUpdate();
	void update();
	
	void setCurrent(const LayerConstPtr &layer);
	void setSelection(const QList<LayerConstPtr> &layers);
	
signals:
	
	void layerAboutToBeInserted(const LayerConstPtr &parent, int index);
	void layerInserted(const LayerConstPtr &parent, int index);
	
	void layerAboutToBeRemoved(const LayerConstPtr &parent, int index);
	void layerRemoved(const LayerConstPtr &parent, int index);
	
	void layerChanged(const LayerConstPtr &layer);
	
	void tilesUpdated(const QPointSet &tileKeys);
	void thumbnailsUpdated();
	
	void currentChanged(const LayerConstPtr &now, const LayerConstPtr &old);
	void selectionChanged(const QList<LayerConstPtr> &selected, const QList<LayerConstPtr> &deselected);
	
	void currentLayerChanged();
	
protected:
	
	void enqueueTileUpdate(const QPointSet &keys);
	LayerPtr mutableRootLayer();
	
	void pushCommand(QUndoCommand *command);
	
private slots:
	
	void updateDirtyThumbnails();
	
	void onCurrentIndexChanged(const QModelIndex &now, const QModelIndex &old);
	void onItemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void onLayerPropertyChanged(const LayerConstPtr &layer);
	
private:
	
	friend class LayerSceneCommand;
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_LAYERSCENE_H
