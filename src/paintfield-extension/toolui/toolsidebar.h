#ifndef FSTOOLPANEL_H
#define FSTOOLPANEL_H

#include <QWidget>

namespace PaintField
{

class ToolManager;

class ToolSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit ToolSidebar(ToolManager *toolManager, QWidget *parent = 0);
	
signals:
	
public slots:
	
private:
};

}

#endif // FSTOOLPANEL_H
