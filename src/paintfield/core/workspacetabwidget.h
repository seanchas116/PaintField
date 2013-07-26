#pragma once

#include "widgets/docktabwidget.h"

namespace PaintField {

class Workspace;

class WorkspaceTabWidget : public DockTabWidget
{
	Q_OBJECT
	
	typedef DockTabWidget super;
	
public:
	
	explicit WorkspaceTabWidget(Workspace *workspace, QWidget *parent = 0);
	~WorkspaceTabWidget();
	
	Workspace *workspace();
	
signals:
	
public slots:
	
protected:
	
private slots:
	
	void onCurrentWorkspaceChanged(Workspace *workspace);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

