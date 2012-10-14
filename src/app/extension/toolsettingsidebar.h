#ifndef FSTOOLSETTINGPANEL_H
#define FSTOOLSETTINGPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include "panel.h"

namespace PaintField
{

class ToolManager;
class ToolFactory;

class ToolSettingSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit ToolSettingSidebar(ToolManager *toolManager, QWidget *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void onCurrentToolFactoryChanged(ToolFactory *factory);
	
private:
	
	QVBoxLayout *_layout;
	QScopedPointer<QWidget> _widget;
};

}

#endif // FSTOOLSETTINGPANEL_H
