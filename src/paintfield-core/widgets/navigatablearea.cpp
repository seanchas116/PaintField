#include <QtGui>
#include "vanishingscrollbar.h"

#include "navigatablearea.h"

namespace PaintField {

NavigatableArea::NavigatableArea(QWidget *parent) :
    QWidget(parent),
    _scrollBarX(new VanishingScrollBar(Qt::Horizontal, this)),
    _scrollBarY(new VanishingScrollBar(Qt::Vertical, this))
{
	updateTransforms();
	
	moveScrollBars();
	
	connect(_scrollBarX, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarXChanged(int)));
	connect(_scrollBarY, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarYChanged(int)));
}

void NavigatableArea::setScale(double value)
{
	if (_scale != value)
	{
		_scale = value;
		updateTransforms();
		emit scaleChanged(value);
	}
}

void NavigatableArea::setViewScale(double scale)
{
	if (_scale != scale)
	{
		_scale = scale;
		emit scaleChanged(scale);
		
		QPoint translation = _backupTranslation * (scale / _backupScale);
		
		if (_translation != translation)
		{
			_translation = translation;
			emit translationChanged(translation);
		}
		
		updateTransforms();
	}
}

void NavigatableArea::setRotation(double value)
{
	if (_rotation != value)
	{
		_rotation = value;
		updateTransforms();
		emit rotationChanged(value);
	}
}

void NavigatableArea::setViewRotation(double rotation)
{
	if (_rotation != rotation)
	{
		_rotation = rotation;
		emit rotationChanged(rotation);
		
		QTransform transform;
		transform.rotate(rotation - _backupRotation);
		
		QPoint translation = transform.map(_backupTranslation);
		
		if (_translation != translation)
		{
			_translation = translation;
			emit translationChanged(_translation);
		}
		
		updateTransforms();
	}
}

void NavigatableArea::setTranslation(const QPoint &value)
{
	QPoint newValue;
	
	newValue.setX(qBound(-_maxAbsTranslation.x(), value.x(), _maxAbsTranslation.x()));
	newValue.setY(qBound(-_maxAbsTranslation.y(), value.y(), _maxAbsTranslation.y()));
	
	if (_translation != newValue)
	{
		_translation = newValue;
		
		_backupTranslation = newValue;
		_backupScale = _scale;
		_backupRotation = _rotation;
		
		updateTransforms();
		emit translationChanged(newValue);
	}
}

void NavigatableArea::updateTransforms()
{
	QTransform transform;
	
	if (_translation != QPoint())
		transform.translate(_translation.x(), _translation.y());
	
	if (_scale != 1.0)
		transform.scale(_scale, _scale);
	
	if (_rotation)
		transform.rotate(_rotation);
	
	_viewCenter = QPoint(geometry().width() / 2, geometry().height() / 2);
	
	_navigatorTransform = transform;
	_transformFromScene = QTransform::fromTranslate(- _sceneSize.width() / 2, - _sceneSize.height() / 2) * _navigatorTransform * QTransform::fromTranslate(_viewCenter.x(), _viewCenter.y());
	_transformToScene = _transformFromScene.inverted();
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	update();
}

void NavigatableArea::onScrollBarXChanged(int value)
{
	int x = -(value - _maxAbsTranslation.x());
	setTranslation(x, _translation.y());
}

void NavigatableArea::onScrollBarYChanged(int value)
{
	int y = -(value - _maxAbsTranslation.y());
	setTranslation(_translation.x(), y);
}

void NavigatableArea::updateScrollBarValue()
{
	_scrollBarX->setValue(-_translation.x() + _maxAbsTranslation.x());
	_scrollBarY->setValue(-_translation.y() + _maxAbsTranslation.y());
}

void NavigatableArea::updateScrollBarRange()
{
	int radius = ceil(hypot(_sceneSize.width(), _sceneSize.height()) * _scale * 0.5);
	
	_maxAbsTranslation = QPoint(radius + this->width(), radius + this->height());
	
	_scrollBarX->setRange(0, 2 * _maxAbsTranslation.x());
	_scrollBarY->setRange(0, 2 * _maxAbsTranslation.y());
	_scrollBarX->setPageStep(this->width());
	_scrollBarY->setPageStep(this->height());
}

void NavigatableArea::moveScrollBars()
{
	int barWidthX = _scrollBarX->totalBarWidth();
	int barWidthY = _scrollBarY->totalBarWidth();
	
	auto widgetRect = QRect(QPoint(), geometry().size());
	
	auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
	auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
	
	_scrollBarX->setGeometry(scrollBarXRect);
	_scrollBarY->setGeometry(scrollBarYRect);
}

void NavigatableArea::wheelEvent(QWheelEvent *event)
{
	QAbstractSlider *scrollBar;
	
	if (event->orientation() == Qt::Horizontal)
		scrollBar = _scrollBarX;
	else
		scrollBar = _scrollBarY;
	
	scrollBar->setValue(scrollBar->value() - event->delta());
}

void NavigatableArea::resizeEvent(QResizeEvent *event)
{
	updateTransforms();
	moveScrollBars();
	event->accept();
}


} // namespace PaintField
