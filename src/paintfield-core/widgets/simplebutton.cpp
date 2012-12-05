#include <QtGui>
#include "../debug.h"

#include "simplebutton.h"

namespace PaintField
{

QIcon SimpleButton::createSimpleIconSet(const QString &basePixmapFile, const QSize &size)
{
	return createSimpleIconSet(QPixmap(basePixmapFile), size);
}

void paintPixmapWithColor(QPixmap *pixmap, const QColor &color)
{
	QPainter painter(pixmap);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.setPen(Qt::NoPen);
	painter.setBrush(color);
	painter.drawRect(pixmap->rect());
}

QIcon SimpleButton::createSimpleIconSet(const QPixmap &originalPixmap, const QSize &size)
{
	QPixmap basePixmap = originalPixmap.scaled(size);
	
	if (basePixmap.isNull())
	{
		PAINTFIELD_DEBUG << "pixmap is null";
		return QIcon();
	}
	
	QPixmap pixmapOff = basePixmap;
	//QPixmap pixmapActiveOff = basePixmap;
	QPixmap pixmapOn = basePixmap;
	//QPixmap pixmapActiveOn = basePixmap;
	
	paintPixmapWithColor(&pixmapOff, QColor(102, 102, 102));
	//fsPaintPixmapWithColor(&pixmapActiveOff, QColor(77, 77, 77));
	paintPixmapWithColor(&pixmapOn, QColor(39, 96, 209));
	//fsPaintPixmapWithColor(&pixmapOn, QColor(255, 60, 60));
	//fsPaintPixmapWithColor(&pixmapActiveOn, QColor(35, 84, 184));
	
	QIcon icon;
	icon.addPixmap(pixmapOff, QIcon::Normal, QIcon::Off);
	//icon.addPixmap(pixmapActiveOff, QIcon::Active, QIcon::Off);
	icon.addPixmap(pixmapOn, QIcon::Normal, QIcon::On);
	//icon.addPixmap(pixmapActiveOn, QIcon::Active, QIcon::On);
	
	return icon;
}

SimpleButton::SimpleButton(QWidget *parent) :
	QToolButton(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setPopupMode(QToolButton::InstantPopup);
}

SimpleButton::SimpleButton(const QIcon &icon, QWidget *parent) :
	SimpleButton(parent)
{
	setIcon(icon);
}

SimpleButton::SimpleButton(const QString &basePixmapFile, const QSize &size, QWidget *parent) :
	SimpleButton(parent)
{
	QIcon icon = createSimpleIconSet(basePixmapFile, size);
	
	if (!icon.isNull())
	{
		setIcon(icon);
		setIconSize(icon.availableSizes().at(0));
	}
}

SimpleButton::SimpleButton(const QString &basePixmapFile, const QSize &size, QObject *obj, const char *slot, QWidget *parent) :
    SimpleButton(basePixmapFile, size, parent)
{
	connect(this, SIGNAL(pressed()), obj, slot);
}

QSize SimpleButton::sizeHint() const
{
	return QSize(_margins.left() + iconSize().width() + _margins.right(), _margins.top() + iconSize().height() + _margins.bottom());
}

void SimpleButton::enterEvent(QEvent *)
{
	_active = true;
	update();
}

void SimpleButton::leaveEvent(QEvent *)
{
	_active = false;
	update();
}

void SimpleButton::paintEvent(QPaintEvent *)
{
	QIcon::Mode mode;
	
	if (isEnabled())
	{
		mode = _active ? QIcon::Active : QIcon::Normal;
	}
	else
	{
		mode = QIcon::Disabled;
	}
	
	QPixmap pixmap = icon().pixmap(iconSize(), mode, isChecked() || isDown() ? QIcon::On : QIcon::Off);
	QPainter painter(this);
	painter.drawPixmap(_margins.left(), _margins.top(), pixmap);
}

}
