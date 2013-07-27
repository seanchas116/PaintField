#include <QtGui>

#include "viewport.h"

using namespace Malachite;

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    _image(800, 600)
{
}

void Viewport::clearPaintable()
{
	if (_mode == ModeImage)
		_image.clear();
	else
		_surface.clear();
}

void Viewport::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	
	switch (_mode)
	{
		default:
		case ModeImage:
		{
			painter.drawImage(QPoint(),  _image.toImageU8().wrapInQImage());
			break;
		}
		case ModeSurface:
		{
			for (const QPoint &key : _surface.keys())
				painter.drawImage(key * Surface::tileWidth(), _surface.tile(key).toImageU8().wrapInQImage());
			break;
		}
	}
}
