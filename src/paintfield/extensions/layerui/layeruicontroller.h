#ifndef FSLAYERUICONTROLLER_H
#define FSLAYERUICONTROLLER_H

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
		ActionMerge
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
	
private slots:
	
	void onSelectionChanged(const QItemSelection &selection);
	
private:
	
	void addLayer(Layer *layer, const QString &description);
	
	struct Data;
	Data *d;
};

}

#endif // FSLAYERUICONTROLLER_H
