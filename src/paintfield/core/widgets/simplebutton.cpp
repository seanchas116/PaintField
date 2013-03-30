#include <QPainter>
#include <QApplication>
#include <QPalette>

#include "simplebutton.h"

namespace PaintField
{

static void paintPixmapWithBrush(QPixmap *pixmap, const QBrush &brush)
{
	QPainter painter(pixmap);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.setPen(Qt::NoPen);
	painter.setBrush(brush);
	painter.drawRect(pixmap->rect());
}

QIcon SimpleButton::createIcon(const QPixmap &basePixmap, const QBrush &onBrush, const QBrush &offBrush)
{
	if (basePixmap.isNull())
	{
		PAINTFIELD_WARNING << "pixmap is null";
		return QIcon();
	}
	
	QPixmap pmOn = basePixmap, pmOff = basePixmap;
	
	
	QBrush actualOnBrush = onBrush, actualOffBrush = offBrush;
	
	if (onBrush.style() == Qt::NoBrush)
		actualOnBrush = QColor(39, 96, 209);
	
	if (offBrush.style() == Qt::NoBrush)
		actualOffBrush = QColor(102, 102, 102);
	
	paintPixmapWithBrush(&pmOn, actualOnBrush);
	paintPixmapWithBrush(&pmOff, actualOffBrush);
	
	QIcon icon;
	icon.addPixmap(pmOff, QIcon::Normal, QIcon::Off);
	icon.addPixmap(pmOn, QIcon::Normal, QIcon::On);
	
	return icon;
}

QIcon SimpleButton::createIcon(const QString &path, const QBrush &onBrush, const QBrush &offBrush)
{
	return createIcon(QPixmap(path), onBrush, offBrush);
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
	QIcon icon = createIcon(basePixmapFile);
	
	if (!icon.isNull())
	{
		setIcon(icon);
		setIconSize(size);
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
