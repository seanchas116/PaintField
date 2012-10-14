#ifndef FSTOOLPANEL_H
#define FSTOOLPANEL_H

#include <QWidget>

namespace PaintField
{

class WorkspaceController;

class ToolPanel : public QWidget
{
	Q_OBJECT
public:
	explicit ToolPanel(WorkspaceController *workspace, QWidget *parent = 0);
	
signals:
	
public slots:
	
private:
	
	WorkspaceController *_workspace;
};

}

#endif // FSTOOLPANEL_H
