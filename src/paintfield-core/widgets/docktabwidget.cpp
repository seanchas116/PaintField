#include <QtGui>
#include "../debug.h"

#include "docktabwidget.h"

#define MIMETYPE_TABINDEX "x-paintfield-tabindex"

namespace PaintField
{

DockTabWidget::DockTabWidget(QWidget *parent) :
	QTabWidget(parent)
{
	auto tabBar = new DockTabBar(this);
	setTabBar(tabBar);
	connect(tabBar, SIGNAL(clicked()), this, SIGNAL(tabClicked()));
	setAcceptDrops(true);
	setDocumentMode(true);
}

DockTabWidget::DockTabWidget(DockTabWidget *other, QWidget *parent) :
	DockTabWidget(parent)
{
	resize(other->size());
	//connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void DockTabWidget::moveTab(DockTabWidget *source, int sourceIndex, DockTabWidget *dest, int destIndex)
{
	if (source == dest && sourceIndex < destIndex)
		destIndex--;
	
	QWidget *widget = source->widget(sourceIndex);
	QString text = source->tabText(sourceIndex);
	
	source->removeTab(sourceIndex);
	
	dest->insertTab(destIndex, widget, text);
	dest->setCurrentIndex(destIndex);
}

bool DockTabWidget::eventIsTabDrag(QDragEnterEvent *event)
{
	return event->mimeData()->hasFormat(MIMETYPE_TABINDEX) && qobject_cast<DockTabBar *>(event->source());
}

void DockTabWidget::deleteIfEmpty()
{
	if (count() == 0 && _autoDeletionEnabled)
	{
		emit willBeAutomaticallyDeleted(this);
		deleteLater();
	}
}

QObject *DockTabWidget::createNew()
{
	return new DockTabWidget(this, 0);
}

void DockTabWidget::focusInEvent(QFocusEvent *)
{
	PAINTFIELD_DEBUG << "focus in";
	emit focusIn();
}

void DockTabWidget::focusOutEvent(QFocusEvent *)
{
	PAINTFIELD_DEBUG << "focus out";
	emit focusOut();
}

void DockTabWidget::closeEvent(QCloseEvent *event)
{
	if (count())
		event->ignore();
}

DockTabBar::DockTabBar(DockTabWidget *tabWidget, QWidget *parent) :
	QTabBar(parent),
	_tabWidget(tabWidget)
{
	Q_ASSERT(tabWidget);
	setAcceptDrops(true);
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
		_dragIndex = tabAt(event->pos());
		if (_dragIndex >= 0)
		{
			_dragStartPos = event->pos();
			_isStartingDrag = true;
		}
	}
	QTabBar::mousePressEvent(event);
}

void DockTabBar::mouseMoveEvent(QMouseEvent *event)
{
	QPoint delta = event->pos() - _dragStartPos;
	if (delta.manhattanLength() < qApp->startDragDistance())
	{
		qApp->setOverrideCursor(Qt::SizeAllCursor);
	}
}

void DockTabBar::mouseReleaseEvent(QMouseEvent *event)
{
	if (_isStartingDrag)
	{
		qApp->setOverrideCursor(QCursor());
		dragDropTab(_dragIndex, mapToGlobal(event->pos()), _dragStartPos - tabRect(_dragIndex).topLeft());
		_isStartingDrag = false;
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
			break;
		}
		widget = widget->parentWidget();
	}
	
	bool dropResult = false;
	
	if (droppable && droppable->tabIsInsertable(_tabWidget, index))	// drop on the existing widget that accepts dock tab drops
		dropResult = qobject_cast<DockTabDroppableInterface *>(droppableWidget)->dropDockTab(_tabWidget, index, droppableWidget->mapFromGlobal(globalPos));
	
	if (!dropResult)	// create new tab widget
	{
		int dstIndex = 0;
		auto dstTabWidget = _tabWidget->createNewTabWidget();
		QRect dstGeom = dstTabWidget->geometry();
		dstGeom.moveTopLeft(globalPos - dragStartOffset);
		dstTabWidget->setGeometry(dstGeom);
		
		_tabWidget->moveTab(index, dstTabWidget, dstIndex);
		
		dstTabWidget->show();
	}
	
	_tabWidget->deleteIfEmpty();
}

bool DockTabBar::dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos)
{
	srcTabWidget->moveTab(srcIndex, _tabWidget, insertionIndexAt(pos));
	return true;
}

}
