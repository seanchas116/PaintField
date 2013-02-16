#include <QtGui>

#include "docktabwidget.h"

#define MIMETYPE_TABINDEX "x-paintfield-tabindex"

namespace PaintField
{

DockTabWidget::DockTabWidget(QWidget *parent) :
	QTabWidget(parent)
{
	new DockTabBar(this);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
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
	return new DockTabWidget();
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
	if (index < 0 && _autoDeletionEnabled)
	{
		emit willBeAutomaticallyDeleted(this);
		deleteLater();
	}
}

DockTabBar::DockTabBar(DockTabWidget *tabWidget, QWidget *parent) :
	QTabBar(parent),
	_tabWidget(tabWidget)
{
	Q_ASSERT(tabWidget);
	
	tabWidget->setTabBar(this);
	connect(this, SIGNAL(clicked()), tabWidget, SIGNAL(tabClicked()));
	
	setDocumentMode(true);
	setExpanding(false);
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
			
			if (droppable->tabIsInsertable(_tabWidget, index))
			{
				bool dropResult = droppable->dropDockTab(_tabWidget, index, droppableWidget->mapFromGlobal(globalPos));
				if (dropResult)
					return;
			}
		}
		widget = widget->parentWidget();
	}
	
	int dstIndex = 0;
	auto dstTabWidget = _tabWidget->createNewTabWidget();
	QRect dstGeom = _tabWidget->geometry();
	dstGeom.moveTopLeft(globalPos - dragStartOffset);
	dstTabWidget->setGeometry(dstGeom);
	
	dstTabWidget->show();
	
	if (!_tabWidget->moveTab(index, dstTabWidget, dstIndex))
		dstTabWidget->deleteLater();
}

bool DockTabBar::dropDockTab(DockTabWidget *srcTabWidget, int srcIndex, const QPoint &pos)
{
	return srcTabWidget->moveTab(srcIndex, _tabWidget, insertionIndexAt(pos));
}

}
