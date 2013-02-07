#include <Malachite/Interval>
#include "brushrasterizer.h"

#include "brushstrokersimplebrush.h"

namespace PaintField {

using namespace Malachite;

BrushStrokerSimpleBrush::BrushStrokerSimpleBrush(Surface *surface) :
    BrushStroker(surface)
{
}

void BrushStrokerSimpleBrush::drawFirst(const TabletInputData &data)
{
	_carryOver = 1;
	drawDab(data.pos, data.pressure);
}

void BrushStrokerSimpleBrush::drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	int count = polygon.size() - 1;
	
	if (count < 1)
		return;
	
	double totalLen;
	QVector<double> lengths = calcLength(polygon, &totalLen);
	
	double totalNormalizeFactor = 1.0 / totalLen;
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) * totalNormalizeFactor;
	
	double pressure = dataStart.pressure;
	double carryOver = _carryOver;
	
	for (int i = 0; i < count; ++i)
		carryOver = drawSegment(polygon[i], polygon[i+1], lengths[i], pressure, pressureNormalized, carryOver);
	
	_carryOver = carryOver;
}

double BrushStrokerSimpleBrush::drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double len, double &pressure, double pressureNormalized, double carryOver)
{
	if (len == 0)
		return carryOver;
	
	if (carryOver > len)
		return carryOver - len;
	
	Vec2D posNormalized = (p2 - p1) / len;
	
	Vec2D pos = p1 + posNormalized * carryOver;
	pressure += pressureNormalized * carryOver;
	
	QRect firstRect = drawDab(pos, pressure);
	
	len -= carryOver;
	
	QRect lastRect;
	
	forever
	{
		len -= 1;
		if (len < 0)
			break;
		
		pos += posNormalized;
		pressure += pressureNormalized;
		
		lastRect = drawDab(pos, pressure);
	}
	
	QRect editedRect;
	
	if (lastRect.isEmpty())
		editedRect = firstRect;
	else
		editedRect = firstRect | lastRect;
	
	addEditedRect(editedRect);
	
	return -len;
}

QRect BrushStrokerSimpleBrush::drawDab(const Vec2D &pos, double pressure)
{
	if (pressure <= 0)
		return QRect();
	
	BrushRasterizerFast ras(pos, radiusBase() * pressure, 2);
	
	while (ras.hasNextScanline())
		drawScanline(ras.nextScanline(), surface());
	
	return ras.boundingRect();
}

static void drawScanlineInTile(Image *tileImage, const QPoint &offset, const BrushScanline &scanline, const Pixel &argb, BlendOp *blendOp)
{
	QPoint pos = scanline.pos - offset;
	
	Q_ASSERT(0 <= pos.y() && pos.y() < Surface::tileWidth());
	
	int count = scanline.count;
	bool isSolid = count < 0;
	if (isSolid)
		count = -count;
	
	Interval interval = Interval(0, Surface::tileWidth()) & Interval(pos.x(), count);
	
	if (interval.isValid())
	{
		auto p = tileImage->pixelPointer(interval.start(), pos.y());
		
		if (isSolid)
		{
			blendOp->blend(count, p, argb);
		}
		else
		{
			auto pcover = scanline.covers + (interval.start() - pos.x());
			blendOp->blend(interval.length(), p, argb, wrapPointer(scanline.covers, count * sizeof(pcover), pcover));
		}
	}
}

void BrushStrokerSimpleBrush::drawScanline(const BrushScanline &scanline, Surface *surface)
{
	int tileY = IntDivision(scanline.pos.y(), Surface::tileWidth()).quot();
	int tileStart = IntDivision(scanline.pos.x(), Surface::tileWidth()).quot();
	int tileEnd = IntDivision(scanline.pos.x() + scanline.count - 1, Surface::tileWidth()).quot();
	
	auto blendOp = BlendMode(BlendMode::SourceOver).op();
	
	for (int tileX = tileStart; tileX <= tileEnd; ++tileX)
	{
		QPoint key(tileX, tileY);
		
		auto tile = getTile(key, surface);
		drawScanlineInTile(tile, key * Surface::tileWidth(), scanline, pixel(), blendOp);
	}
}

Image *BrushStrokerSimpleBrush::getTile(const QPoint &key, Surface *surface)
{
	if (_lastTile && _lastKey == key)
	{
		return _lastTile;
	}
	else
	{
		_lastKey = key;
		_lastTile = &surface->tileRef(key);
		return _lastTile;
	}
}

void BrushStrokerSimpleBrush::loadSettings(const QVariantMap &settings)
{
	Q_UNUSED(settings)
}

BrushStrokerSimpleBrushFactory::BrushStrokerSimpleBrushFactory(QObject *parent) :
    BrushStrokerFactory(parent)
{
	
}

QString BrushStrokerSimpleBrushFactory::name() const
{
	return "paintfield.brush.simpleBrush";
}

QVariantMap BrushStrokerSimpleBrushFactory::defaultSettings() const
{
	return QVariantMap();
}

BrushStroker *BrushStrokerSimpleBrushFactory::createStroker(Surface *surface)
{
	return new BrushStrokerSimpleBrush(surface);
}

} // namespace PaintField
