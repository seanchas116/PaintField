#include "selectionstroker.h"
#include "paintfield/core/selection.h"

namespace PaintField {

SelectionStroker::SelectionStroker(Selection *selection) :
	mSelection(selection)
{
}

void SelectionStroker::drawFirst(const TabletInputData &data)
{
	double radius = data.pressure * radiusBase();
	QPainterPath path;
	path.addEllipse(data.pos, radius, radius);
	this->drawPath(path);
}

void SelectionStroker::drawInterval(
	const Malachite::Polygon &polygon, const QVector<double> &lengths, double totalLength,
	const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) / totalLength;
	double pressure = dataStart.pressure;

	auto count = polygon.size();
	double radius = pressure * radiusBase();

	QPainterPath path;
	path.setFillRule(Qt::WindingFill);

	for (int i = 1; i < count; ++i) {

		if (lengths.at(i-1) == 0)
			continue;
		pressure += pressureNormalized * lengths.at(i-1);

		double prevRadius = radius;
		radius = pressure * radiusBase();

		auto quad = tangentQuadrangle(prevRadius, polygon.at(i-1), radius, polygon.at(i), lengths.at(i-1)).toQPolygonF();
		path.addPolygon(quad);
		path.addEllipse(polygon.at(i), radius, radius);
	}

	this->drawPath(path);
}

void SelectionStroker::drawPath(const QPainterPath &path)
{
	auto surface = mSelection->surface();

	auto keys = Malachite::Surface::rectToKeys(path.boundingRect().toAlignedRect());
	for (const QPoint &key : keys) {
		QPainterPath rectPath;
		rectPath.addRect(Malachite::Surface::keyToRect(key));
		auto dividedPath = path & rectPath;

		if (dividedPath.isEmpty())
			continue;

		auto &tile = surface.tileRef(key);
		QPainter painter(&tile.qimage());
		painter.setPen(Qt::NoPen);
		painter.translate(-key * Malachite::Surface::tileWidth());
		painter.drawPath(path);
	}

	mSelection->updateSurface(surface, keys);
}


} // namespace PaintField
