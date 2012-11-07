#ifndef TABDOCUMENTAREA_H
#define TABDOCUMENTAREA_H

#include <QWidget>
#include <QSplitter>
#include "../util.h"
#include "docktabwidget.h"

namespace PaintField
{

class TabDocumentAreaNode;

class TabDocumentArea : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit TabDocumentArea(DockTabWidget *tabWidget, QWidget *parent = 0);
	~TabDocumentArea();
	
public slots:
	
	void addTab(QWidget *tab, const QString &title, const QString &toolTipText);
	void takeTab(QWidget *tab);
	void setCurrentTab(QWidget *tab);
	
	void splitCurrent(Qt::Orientation orientation);
	void closeCurrent();
	
signals:
	
	void currentTabChanged(QWidget *tab);
	
protected:
	
private slots:
	
	void onSplitActivated();
	
private:
	
	void updateCurrentTab();
	
	DockTabWidget *tabWidgetForTab(QWidget *tab);
	
	void setCurrentNode(TabDocumentAreaNode *node);
	
	QScopedPointer<TabDocumentAreaNode> _rootNode;
	TabDocumentAreaNode *_currentNode = 0;
	DockTabWidget *_currentTabWidget = 0;
	QWidget *_currentTab = 0;
};

}

#endif // TABDOCUMENTAREA_H
