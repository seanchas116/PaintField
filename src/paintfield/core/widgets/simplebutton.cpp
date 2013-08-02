#include <QPainter>
#include <QApplication>
#include <QPalette>

#include "simplebutton.h"

namespace PaintField
{

struct SimpleButton::Data
{
	QMargins margins;
	bool active = false;
	bool pressable = true;
};

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
	QToolButton(parent),
	d(new Data)
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

SimpleButton::~SimpleButton()
{
	delete d;
}

QSize SimpleButton::sizeHint() const
{
	return QSize(d->margins.left() + iconSize().width() + d->margins.right(), d->margins.top() + iconSize().height() + d->margins.bottom());
}

void SimpleButton::setPressable(bool pressable)
{
	d->pressable = pressable;
}

bool SimpleButton::isPressable() const
{
	return d->pressable;
}

void SimpleButton::setMargins(const QMargins &margins)
{
	d->margins = margins;
}

QMargins SimpleButton::margins() const
{
	return d->margins;
}

bool SimpleButton::hitButton(const QPoint &pos) const
{
	if (d->pressable)
		return QToolButton::hitButton(pos);
	else
		return false;
}

void SimpleButton::enterEvent(QEvent *)
{
	d->active = true;
	update();
}

void SimpleButton::leaveEvent(QEvent *)
{
	d->active = false;
	update();
}

void SimpleButton::paintEvent(QPaintEvent *)
{
	QIcon::Mode mode;
	
	if (isEnabled())
	{
		mode = d->active ? QIcon::Active : QIcon::Normal;
	}
	else
	{
		mode = QIcon::Disabled;
	}
	
	QPixmap pixmap = icon().pixmap(iconSize(), mode, (isChecked() || isDown()) ? QIcon::On : QIcon::Off);
	QPainter painter(this);
	painter.drawPixmap(d->margins.left(), d->margins.top(), pixmap);
}

}
