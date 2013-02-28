#include <QPainter>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QContextMenuEvent>

#include "colorbutton.h"

namespace PaintField
{

using namespace Malachite;

struct ColorButton::Data
{
	Color color;
	QAction *copyAction = 0, *pasteAction = 0;
};

ColorButton::ColorButton(QWidget *parent) :
	QAbstractButton(parent),
	d(new Data)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setCheckable(true);
	
	{
		auto a = new QAction(tr("Copy"), this);
		a->setShortcut(QKeySequence::Copy);
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(copyColor()));
		addAction(a);
		d->copyAction = a;
	}
	
	{
		auto a = new QAction(tr("Paste"), this);
		a->setShortcut(QKeySequence::Paste);
		a->setShortcutContext(Qt::WidgetShortcut);
		connect(a, SIGNAL(triggered()), this, SLOT(pasteColor()));
		addAction(a);
		d->pasteAction = a;
	}
	
	setFocusPolicy(Qt::ClickFocus);
}

ColorButton::~ColorButton()
{
	delete d;
}

QSize ColorButton::sizeHint() const
{
	return QSize(ButtonSize + 2 * ButtonMargin, ButtonSize + 2 * ButtonMargin);
}

void ColorButton::setColor(const Color &c)
{
	if (d->color != c)
	{
		d->color = c;
		emit colorChanged(c);
		update();
	}
}

Color ColorButton::color() const
{
	return d->color;
}

void ColorButton::copyColor()
{
	auto mime = new QMimeData;
	mime->setColorData(d->color.toQColor());
	qApp->clipboard()->setMimeData(mime);
}

void ColorButton::pasteColor()
{
	auto mime = qApp->clipboard()->mimeData();
	if (!mime || !mime->hasColor())
		return;
	
	auto color = qvariant_cast<QColor>(mime->colorData());
	setColor(Color::fromQColor(color));
}

void ColorButton::contextMenuEvent(QContextMenuEvent *event)
{
	auto globalPos = mapToGlobal(event->pos());
	
	QMenu menu;
	
	menu.addAction(d->copyAction);
	menu.addAction(d->pasteAction);
	
	menu.exec(globalPos);
}

void ColorButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	QRect buttonRect(ButtonMargin, ButtonMargin, ButtonSize, ButtonSize);
	
	painter.setPen(Qt::NoPen);
	
	painter.setBrush(Qt::white);
	painter.drawRect(buttonRect);
	
	painter.setBrush(d->color.toQColor());
	painter.drawRect(buttonRect);
	
	if (d->color.alpha() != 1.0)
	{
		auto opaqueColor = d->color;
		opaqueColor.setAlpha(1.0);
		
		painter.setBrush(opaqueColor.toQColor());
		QVector<QPoint> polygon = { buttonRect.topLeft(), buttonRect.topLeft() + QPoint(buttonRect.width(), 0), buttonRect.topLeft() + QPoint(0, buttonRect.height()) };
		painter.drawPolygon(polygon);
	}
	
	if (isChecked())
	{
		painter.setBrush(Qt::NoBrush);
		//painter.setPen(palette().color(QPalette::Active, QPalette::Highlight));
		painter.setPen(QPen(QColor(39, 96, 209), 2.0));
		painter.setRenderHint(QPainter::Antialiasing, true);
		
		painter.drawRect(buttonRect);
	}
}

}
