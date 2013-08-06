#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QMimeData>

#include "appcontroller.h"
#include "cursorstack.h"

#include "docktabwidget.h"

#define MIMETYPE_TABINDEX "x-paintfield-tabindex"

namespace PaintField
{

struct DockTabWidget::Data
{
	bool autoDeletionEnabled = false;
	bool floating = false;
	QWidget *baseWindow = 0;
};

DockTabWidget::DockTabWidget(QWidget *baseWindow, QWidget *parent) :
	QTabWidget(parent),
	d(new Data)
{
	d->baseWindow = baseWindow;
	
	new DockTabBar(this, 0);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

DockTabWidget::~DockTabWidget()
{
	delete d;
}

void DockTabWidget::makeFloating()
{
	PAINTFIELD_DEBUG << d->baseWindow;
	hide();
	setParent(d->baseWindow);
	setWindowFlags(Qt::Tool);
	show();
	d->floating = true;
}

bool DockTabWidget::isFloating() const
{
	return d->floating;
}

void DockTabWidget::setAutoDeletionEnabled(bool x)
{
	d->autoDeletionEnabled = x;
}

bool DockTabWidget::isAutoDeletionEnabled() const
{
	return d->autoDeletionEnabled;
}


QWidgetList DockTabWidget::tabs()
{
	QWidgetList list;
	
	for (int i = 0; i < count(); ++i)
		list << widget(i);
	
	return list;
}

void DockTabWidget::requestCloseAllTabs()
{
	if (count())
		emit closeAllTabsRequested();
}

bool DockTabWidget::moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex)
{
	if (!dest->tabIsInsertable(source, sourceIndex))
		return false;
	
	if (source == dest && source->count() == 1)
		return true;
	
	if (source == dest && sourceIndex < destIndex)
		destIndex--;
	
	QWidget *widget = source->widget(sourceIndex);
	QString text = source->tabText(sourceIndex);
	
	emit source->tabAboutToBeMovedOut(widget);
	source->removeTab(sourceIndex);
	
	dest->insertTab(destIndex, widget, text);
	dest->setCurrentIndex(destIndex);
	emit dest->tabMovedIn(widget);
	
	return true;
}

bool DockTabWidget::eventIsTabDrag(QDragEnterEvent *event)
{
	return event->mimeData()->hasFormat(MIMETYPE_TABINDEX) && qobject_cast<DockTabBar *>(event->source());
}

QObject *DockTabWidget::createNew()
{
	auto w = new DockTabWidget(d->baseWindow, 0);
	w->setAutoDeletionEnabled(d->autoDeletionEnabled);
	return w;
}

void DockTabWidget::mousePressEvent(QMouseEvent *event)
{
	PAINTFIELD_DEBUG;
	event->ignore();
}

void DockTabWidget::closeEvent(QCloseEvent *event)
{
	requestCloseAllTabs();
	
	if (count())
		event->ignore();
	else
		event->accept();
}

void DockTabWidget::onCurrentChanged(int index)
{
	if (index < 0 && d->autoDeletionEnabled)
	{
		emit willBeAutomaticallyDeleted(this);
		deleteLater();
	}
}

struct DockTabBar::Data
{
	DockTabWidget *tabWidget = 0;
	bool isStartingDrag = false, isCursorOverridden = false;
	QPoint dragStartPos;
	int dragIndex;
};

DockTabBar::DockTabBar(DockTabWidget *tabWidget, QWidget *parent) :
	QTabBar(parent),
	d(new Data)
{
	d->tabWidget = tabWidget;
	
	Q_ASSERT(tabWidget);
	
	tabWidget->setTabBar(this);
	connect(this, SIGNAL(clicked()), tabWidget, SIGNAL(tabClicked()));
	
	setDocumentMode(true);
}

DockTabBar::~DockTabBar()
{
	delete d;
}

DockTabWidget *DockTabBar::tabWidget()
{
	return d->tabWidget;
}

bool DockTabBar::tabIsInsertable(DockTabWidget *src, int srcIndex)
{
	return d->tabWidget->tabIsInsertable(src, srcIndex);
}

int DockTabBar::insertionIndexAt(const QPoint &pos)
{
	int index = count();
	for (int i = 0; i < count(); ++i)
	{
		QRect rect = tabRect(i);
		QRect rect1(rect.x(), rect.y(), rect.width() / 2, rect.height());
		QRect rect2(rect.x() + rect1.width(), rect.y(), rect.width() - rect1.width(), rect.height());
		if (rect1.contains(pos))
		{
			index = i;
			break;
		}
		if (rect2.contains(pos))
		{
			index = i + 1;
			break;
		}
	}
	return index;
}

void DockTabBar::mousePressEvent(QMouseEvent *event)
{
	emit clicked();
	
	if (event->button() == Qt::LeftButton)
	{
		d->dragIndex = tabAt(event->pos());
		if (d->dragIndex >= 0)
		{
			d->dragStartPos = event->pos();
			d->isStartingDrag = true;
		}
	}
	QTabBar::mousePressEvent(event);
}

static const QString cursorId = "paintfield.docktab.drag";

void DockTabBar::mouseMoveEvent(QMouseEvent *event)
{
	if (d->isStartingDrag)
	{
		QPoint delta = event->pos() - d->dragStartPos;
		if (delta.manhattanLength() >= qApp->startDragDistance())
		{
			appController()->cursorStack()->add(cursorId, Qt::SizeAllCursor);
			d->isCursorOverridden = true;
		}
	}
}

void DockTabBar::mouseReleaseEvent(QMouseEvent *event)
{
	if (d->isCursorOverridden)
	{
		appController()->cursorStack()->remove(cursorId);
		d->isCursorOverridden = false;
	}
	
	if (d->isStartingDrag && (event->pos() - d->dragStartPos).manhattanLength() >= qApp->startDragDistance())
	{
		dragDropTab(d->dragIndex, mapToGlobal(event->pos()), d->dragStartPos - tabRect(d->dragIndex).topLeft());
		d->isStartingDrag = false;
	}
}

void DockTabBar::dragDropTab(int index, const QPoint &globalPos, const QPoint &dragStartOffset)
{
	QWidget *widget = qApp->widgetAt(globalPos);
	
	QWidget *droppableWidget = 0;
	DockTabDroppableInterface *droppable = 0;
	
	while (widget)
	{
		droppable = qobject_cast<DockTabDroppableInterface *>(widget);
		
		if (droppable)
		{
			droppableWidget = widget;
			
			if (droppable->tabIsInsertable(d->tabWidget, index))
			{
				bool dropResult = droppable->dropDockTab(d->tabWidget, index, droppableWidget->mapFromGlobal(globalPos));
				if (dropResult)
					return;
			}
		}
		widget = widget->parentWidget();
	}
	
	// dropping outside tab widget is temporarily disabled
	/*
	int dstIndex = 0;
	auto dstTabWidget = d->tabWidget->createNewTabWidget();
	QRect dstGeom = d->tabWidget->geometry();
	dstGeom.moveTopLeft(globalPos - dragStartOffset);
	dstTabWidget->setGeometry(dstGeom);
	
	dstTabWidget->makeFloating();
	//dstTabWidget->show();
	
	if (!d->tabWidget->moveTab(index, dstTabWidget, dstIndex))
		dstTabWidget->deleteLater();
	*/
}

bool DockTabBar::dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos)
{
	return srcTabWidget->moveTab(srcIndex, d->tabWidget, insertionIndexAt(pos));
}

}
