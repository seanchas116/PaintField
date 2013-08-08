#pragma once

#include "paintfield/core/layer.h"
#include "paintfield/core/widgets/loosespinbox.h"

class QHBoxLayout;
class QVBoxLayout;
class QMenu;
class QItemSelection;
class QModelIndex;
class QComboBox;
class QFormLayout;

namespace PaintField
{

class Document;
class Workspace;
class Canvas;
class LayerUIController;

class LooseSlider;
class SimpleButton;

class LayerTreeSidebar : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent = 0);
	~LayerTreeSidebar();
	
	bool eventFilter(QObject *object, QEvent *event);
	
public slots:
	
signals:
	
protected:
	
private slots:
	
	void showViewContextMenu(const QPoint &pos);
	
private:
	
	void createForms();
	
	struct Data;
	Data *d;
};

}

