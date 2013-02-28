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

ColorButton::ColorButton(QWidget *parent) :
	QAbstractButton(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setCheckable(true);
}

QSize ColorButton::sizeHint() const
{
	return QSize(ButtonSize + 2 * ButtonMargin, ButtonSize + 2 * ButtonMargin);
}

void ColorButton::setColor(const Color &c)
{
	_color = c;
	update();
}

void ColorButton::copyColor()
{
	auto mime = new QMimeData;
	mime->setColorData(_color.toQColor());
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
	
	auto copyAction = new QAction(tr("Copy"), &menu);
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyColor()));
	
	auto pasteAction = new QAction(tr("Paste"), &menu);
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteColor()));
	
	menu.addAction(copyAction);
	menu.addAction(pasteAction);
	
	menu.exec(globalPos);
}

void ColorButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	QRect buttonRect(ButtonMargin, ButtonMargin, ButtonSize, ButtonSize);
	
	painter.setPen(Qt::NoPen);
	
	painter.setBrush(Qt::white);
	painter.drawRect(buttonRect);
	
	painter.setBrush(_color.toQColor());
	painter.drawRect(buttonRect);
	
	if (_color.alpha() != 1.0)
	{
		auto opaqueColor = _color;
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
