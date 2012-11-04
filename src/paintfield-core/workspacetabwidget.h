#ifndef WORKSPACETABWIDGET_H
#define WORKSPACETABWIDGET_H

#include <QWidget>
#include "widgets/docktabwidget.h"
#include "workspaceview.h"

namespace PaintField
{

class WorkspaceTabWidget : public DockTabWidget
{
	Q_OBJECT
public:
	
	enum Type
	{
		TypeSidebar,
		TypeCanvas
	};
	
	WorkspaceTabWidget(WorkspaceView *workspaceView, Type type, QWidget *parent = 0);
	WorkspaceTabWidget(WorkspaceTabWidget *other, QWidget *parent = 0);
	
	bool isInsertableFrom(DockTabWidget *other) override;
	DockTabWidget *createAnother(QWidget *parent) override;
	
private:
	
	WorkspaceView *_workspaceView;
	Type _type;
};

}

#endif // WORKSPACETABWIDGET_H
