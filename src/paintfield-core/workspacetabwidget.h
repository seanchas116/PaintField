#ifndef PAINTFIELD_WORKSPACETABWIDGET_H
#define PAINTFIELD_WORKSPACETABWIDGET_H

#include "widgets/docktabwidget.h"

namespace PaintField {

class WorkspaceController;

class WorkspaceTabWidget : public DockTabWidget
{
	Q_OBJECT
	
	typedef DockTabWidget super;
	
public:
	
	explicit WorkspaceTabWidget(WorkspaceController *workspace, QWidget *parent = 0);
	~WorkspaceTabWidget();
	
	bool isFloating() const;
	void setFloating(bool floating);
	
	WorkspaceController *workspace();
	
signals:
	
public slots:
	
protected:
	
	bool event(QEvent *event);
	
private slots:
	
	void onCurrentWorkspaceChanged(WorkspaceController *workspace);
	
private:
	
	class Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_WORKSPACETABWIDGET_H
