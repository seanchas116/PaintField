#include <QtGui>
#include "../util.h"

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

void DockTabWidget::decodeTabDropEvent(QDropEvent *event, DockTabWidget **p_tabWidget, int *p_index)
{
	DockTabBar *tabBar = qobject_cast<DockTabBar *>(event->source());
	if (!tabBar)
	{
		*p_tabWidget = nullptr;
		*p_index = 0;
		return;
	}
	
	QByteArray data = event->mimeData()->data(MIMETYPE_TABINDEX);
	QDataStream stream(&data, QIODevice::ReadOnly);
	
	int index;
	stream >> index;
	
	*p_tabWidget = tabBar->tabWidget();
	*p_index = index;
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
		_dragStartPos = event->pos();
		_isStartingDrag = true;
	}
	QTabBar::mousePressEvent(event);
}

void DockTabBar::mouseMoveEvent(QMouseEvent *event)
{
	if (!_isStartingDrag)
		return;
	
	if (!event->buttons() & Qt::LeftButton)
		return;
	
	if ((event->pos() - _dragStartPos).manhattanLength() < QApplication::startDragDistance())
		return;
	
	int index = tabAt(event->pos());
	
	if (index < 0)
		return;
	
	// create data
	
	QMimeData *mimeData = new QMimeData;
	
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream << index;
	
	mimeData->setData(MIMETYPE_TABINDEX, data);
	
	// create pixmap
	
	QRect rect = tabRect(index);
	QPixmap pixmap(rect.size());
	
	render(&pixmap, QPoint(), QRegion(rect));
	
	// exec drag
	
	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	QPoint offset = _dragStartPos - rect.topLeft();
	drag->setHotSpot(offset);
	Qt::DropAction dropAction = drag->exec(Qt::MoveAction | Qt::IgnoreAction);
	
	if (dropAction != Qt::MoveAction)	// drop is not accepted
	{
		DockTabWidget *newTabWidget = _tabWidget->createNewTabWidget();
		if (!newTabWidget->isInsertableFrom(_tabWidget))
		{
			newTabWidget->deleteLater();
		}
		else
		{
			DockTabWidget::moveTab(_tabWidget, index, newTabWidget, 0);
			
			QRect newGeometry = newTabWidget->geometry();
			newGeometry.moveTopLeft(QCursor::pos() - offset);
			newTabWidget->setGeometry(newGeometry);
			newTabWidget->show();
		}
	}
	
	_isStartingDrag = false;
	_tabWidget->deleteIfEmpty();
}

void DockTabBar::dragMoveEvent(QDragMoveEvent *event)
{
	event->accept();
}

void DockTabBar::dragEnterEvent(QDragEnterEvent *event)
{
	if (DockTabWidget::eventIsTabDrag(event))
		event->acceptProposedAction();
}

void DockTabBar::dropEvent(QDropEvent *event)
{
	DockTabWidget *oldTabWidget;
	int oldIndex;
	DockTabWidget::decodeTabDropEvent(event, &oldTabWidget, &oldIndex);
	
	if (oldTabWidget && _tabWidget->isInsertableFrom(oldTabWidget))
	{
		int newIndex = insertionIndexAt(event->pos());
		DockTabWidget::moveTab(oldTabWidget, oldIndex, _tabWidget, newIndex);
		event->acceptProposedAction();
	}
}

}
