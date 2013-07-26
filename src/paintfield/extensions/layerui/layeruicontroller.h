#pragma once

#include <QObject>
#include "paintfield/core/workspace.h"
#include "paintfield/core/canvas.h"

class QItemSelection;

namespace PaintField
{

class LayerUIController : public QObject
{
	Q_OBJECT
public:
	
	LayerUIController(Document *document, QObject *parent);
	~LayerUIController();
	
	enum ActionType
	{
		ActionImport,
		ActionNewRaster,
		ActionNewGroup,
		ActionRemove,
		ActionCopy,
		ActionCut,
		ActionPaste,
		ActionMerge,
		ActionRasterize
	};
	
	QAction *action(ActionType type);
	QList<QAction *> actions();
	
	Document *document();
	
signals:
	
public slots:
	
	void importLayer();
	void newRasterLayer();
	void newGroupLayer();
	void removeLayers();
	void mergeLayers();
	void rasterizeLayers();
	
	void copyLayers();
	void cutLayers();
	void pasteLayers();
	
private slots:
	
	void onSelectionChanged();
	
private:
	
	void copyOrCutLayers(bool cut);
	
	void addLayers(const QList<LayerRef> &layers, const QString &description);
	
	struct Data;
	Data *d;
};

}

