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
	LayerScene(const LayerList &layers, Document *document);
	~LayerScene();
	
	void addLayers(const LayerList &layers, const LayerRef &parent, int index, const QString &description);
	void removeLayers(const LayerRefList &refs, const QString &description = QString());
	void moveLayers(const LayerRefList &refs, const LayerRef &parent, int index);
	void copyLayers(const LayerRefList &refs, const LayerRef &parent, int index);
	void mergeLayers(const LayerRef &parent, int index, int count);
	
	void editLayer(const LayerRef &ref, LayerEdit *edit, const QString &description);
	void setLayerProperty(const LayerRef &ref, const QVariant &data, int role, const QString &description = QString());
	
	LayerRef rootLayer() const;
	Document *document();
	
	LayerItemModel *itemModel();
	QItemSelectionModel *itemSelectionModel();
	
	LayerRef current() const;
	LayerRefList selection() const;
	
public slots:
	
	void abortThumbnailUpdate();
	void update();
	
	void setCurrent(const LayerRef &layer);
	void setSelection(const LayerRefList &layers);
	
signals:
	
	void layerAboutToBeInserted(const LayerRef &parent, int index);
	void layerInserted(const LayerRef &parent, int index);
	
	void layerAboutToBeRemoved(const LayerRef &parent, int index);
	void layerRemoved(const LayerRef &parent, int index);
	
	void layerPropertyChanged(const LayerRef &layer);
	
	void tilesUpdated(const QPointSet &tileKeys);
	void thumbnailsUpdated();
	
	void currentChanged(const LayerRef &now, const LayerRef &old);
	void selectionChanged(const LayerRefList &selected, const LayerRefList &deselected);
	
protected:
	
	void enqueueTileUpdate(const QPointSet &keys);
	Layer *mutableRootLayer();
	
	void pushCommand(QUndoCommand *command);
	
private slots:
	
	void updateDirtyThumbnails();
	
	void onCurrentIndexChanged(const QModelIndex &now, const QModelIndex &old);
	void onItemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	
private:
	
	friend class LayerSceneCommand;
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_LAYERSCENE_H
