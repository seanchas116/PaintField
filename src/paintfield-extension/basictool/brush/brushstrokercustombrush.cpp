#include "brushstrokercustombrush.h"

using namespace Malachite;

namespace PaintField {

void BrushStrokerCustomBrush::drawFirst(const TabletInputData &data)
{
	_carryOver = 1;
	drawDab(data);
}

void BrushStrokerCustomBrush::drawInterval(const Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	int count = polygon.size() - 1;
	if (count < 1)
		return;
	
	double totalLength;
	QVector<double> lengths = calcLength(polygon, &totalLength);
	
	double totalNormalizeFactor = 1.0 / totalLength;
	
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) * totalNormalizeFactor;
	double rotationNormalized = (dataEnd.rotation - dataStart.rotation) * totalNormalizeFactor;
	double tangentialPressureNormalized = (dataEnd.tangentialPressure - dataStart.tangentialPressure) * totalNormalizeFactor;
	Vec2D tiltNormalized = (dataEnd.tilt - dataStart.tilt) * totalNormalizeFactor;
	
	TabletInputData data = dataStart;
	
	for (int i = 0; i < count; ++i)
		drawSegment(polygon.at(i+1), polygon.at(i), lengths[i], data, pressureNormalized, rotationNormalized, tangentialPressureNormalized, tiltNormalized);
}

void BrushStrokerCustomBrush::drawSegment(const Vec2D &p1, const Vec2D &p2, double length, TabletInputData &data, double pressureNormalized, double rotationNormalized, double tangentialPressureNormalized, const Vec2D &tiltNormalized)
{
	if (length == 0)
		return;
	
	if (_carryOver > length)
	{
		_carryOver -= length;
		return;
	}
	
	Vec2D dispNormalized = (p2 - p1) / length;
	
	data.pos = p1;
	
	data.pos += dispNormalized * _carryOver;
	data.pressure += pressureNormalized * _carryOver;
	data.rotation += rotationNormalized * _carryOver;
	data.tangentialPressure += tangentialPressureNormalized * _carryOver;
	data.tilt += tiltNormalized * _carryOver;
	
	drawDab(data);
	
	length -= _carryOver;
	
	forever
	{
		length -= 1;
		
		if (length < 0)
			break;
		
		data.pos += dispNormalized;
		data.pressure += pressureNormalized;
		data.rotation += rotationNormalized;
		data.tangentialPressure += tangentialPressureNormalized;
		data.tilt += tiltNormalized;
		
		drawDab(data);
	}
	
	_carryOver = -length;
}

Image BrushStrokerCustomBrush::drawDabImage(const TabletInputData &data, QRect *rect)
{
	Q_ASSERT(data.pressure > 0);
	
	Vec2D radiusVec;
	radiusVec.x = radiusBase() * data.pressure;
	radiusVec.y = radiusVec.x * (1.0 - _setting.flattening);
	
	_lastMinorRadius = radiusVec.y;
	
	//qDebug() << "radius" << radiusVec.x << radiusVec.y;
	
	QPainterPath ellipse;
	ellipse.addEllipse(data.pos, radiusVec.x, radiusVec.y);
	
	QRect dabRect;
	
	if (_setting.rotation)
	{
		QTransform rotation;
		rotation.rotate(_setting.rotation);
		ellipse = rotation.map(ellipse);
		
		dabRect = ellipse.boundingRect().toAlignedRect();
	}
	else
	{
		dabRect = QRectF(QPointF(data.pos - radiusVec), QSizeF(2.0 * radiusVec)).toAlignedRect();
	}
	
	Image dabImage(dabRect.size());
	dabImage.clear();
	
	Painter dabPainter(&dabImage);
	
	dabPainter.translateShape(-dabRect.topLeft());
	
	if (_setting.tableWidth == 1 && _setting.tableHeight == 1)
	{
		// no gradient
		dabPainter.setArgb(argb());
	}
	else
	{
		ArgbGradient gradient;
		gradient.addStop(0, argb());
		gradient.addStop(_setting.tableWidth, argb() * _setting.tableHeight);
		gradient.addStop(1, Vec4F(0));
		
		dabPainter.setBrush(Malachite::Brush::fromRadialGradient(gradient, data.pos, radiusVec));
	}
	
	dabPainter.drawPath(ellipse);
	
	dabPainter.end();
	
	*rect = dabRect;
	return dabImage;
}

void BrushStrokerCustomBrush::drawDab(const TabletInputData &data)
{
	//qDebug() << "drawing dab" << data.pos.x << data.pos.y;
	
	if (data.pressure <= 0)
		return;
	
	SurfacePainter painter(surface());
	
	QRect dabRect;
	Image dabImage = drawDabImage(data, &dabRect);
	
	//qDebug() << "dab rect" << dabRect;
	
	// erase
	
	if (_setting.erasing)
	{
		painter.setBlendMode(BlendMode::DestinationOut);
		painter.setOpacity(_setting.erasing);
		painter.drawImage(dabRect.topLeft(), dabImage);
	}
	
	painter.setBlendMode(BlendMode::SourceOver);
	painter.drawImage(dabRect.topLeft(), dabImage);
	//painter.drawEllipse(dabRect);
	
	painter.flush();
	
	addEditedKeys(Surface::divideRect(dabRect));
}

void BrushStrokerCustomBrush::loadSettings(const QVariantMap &settings)
{
	
}

QVariantMap BrushSourceBrushFactory::defaultSettings() const
{
	QVariantMap settings;
	
	return settings;
}

BrushStroker *BrushSourceBrushFactory::createStroker(Surface *surface)
{
	return new BrushStrokerCustomBrush(surface);
}

}
