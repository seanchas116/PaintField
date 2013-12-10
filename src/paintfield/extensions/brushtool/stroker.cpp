#include "stroker.h"
#include <Malachite/Curves>
#include <Malachite/CurveSubdivision>

using namespace Malachite;

namespace PaintField {

Stroker::Stroker() :
	mSegment(2)
{
}

void Stroker::moveTo(const TabletInputData &originalData)
{
	auto data = originalData;
	filterData(data);

	clearLastEditedKeys();

	mCount = 0;
	mDataEnd = data;

	if (mSmoothed)
	{
		mDataPrev = data;
		mDataStart = data;
	}
	else
	{
		drawFirst(data);
	}
}

void Stroker::lineTo(const TabletInputData &originalData)
{
	auto data = originalData;
	filterData(data);

	mCount += 1;

	if (mSmoothed)
	{
		if (mCount >= 2)
		{
			auto polygon = CurveSubdivision(Curve4::fromBSpline(mDataPrev.pos, mDataStart.pos, mDataEnd.pos, data.pos)).polygon();

			if (mCount == 2)
			{
				auto firstData = mDataStart;
				firstData.pos = polygon.first();
				drawFirst(firstData);
			}

			drawInterval(polygon, mDataStart, mDataEnd);
		}

		mDataPrev = mDataStart;
		mDataStart = mDataEnd;
		mDataEnd = data;
	}
	else
	{
		mDataStart = mDataEnd;
		mDataEnd = data;

		if (mDataStart.pos == mDataEnd.pos)
			return;

		mSegment[0] = mDataStart.pos;
		mSegment[1] = mDataEnd.pos;

		drawInterval(mSegment, mDataStart, mDataEnd);
	}
}

void Stroker::end()
{
	if (mSmoothed)
	{
		lineTo(mDataEnd);
	}
}

void Stroker::addEditedKeys(const QHash<QPoint, QRect> &keysWithRects)
{
	for (auto iter = keysWithRects.begin(); iter != keysWithRects.end(); ++iter)
		addEditedKey(iter.key(), iter.value());
}

void Stroker::addEditedKey(const QPoint &key, const QRect &rect)
{
	mLastEditedKeysWithRects[key] |= rect;
	mTotalEditedKeys << key;
}

void Stroker::addEditedRect(const QRect &rect)
{
	auto topLeftKey = Surface::keyForPixel(rect.topLeft());
	auto bottomRightKey = Surface::keyForPixel(rect.bottomRight());

	for (int y = topLeftKey.y(); y <= bottomRightKey.y(); ++y)
	{
		for (int x = topLeftKey.x(); x <= bottomRightKey.x(); ++x)
		{
			QPoint key(x, y);
			auto keyRect = rect.translated(key * -Surface::tileWidth()) & Surface::keyToRect(0, 0);
			addEditedKey(key, keyRect);
		}
	}
}

std::tuple<QVector<double>, double> Stroker::polygonLengths(const Polygon &polygon)
{
	double total = 0;

	int count = polygon.size() - 1;
	if (count < 0)
		return QVector<double>();

	QVector<double> lengths(count);

	for (int i = 0; i < count; ++i)
	{
		double length = (polygon.at(i+1) - polygon.at(i)).length();
		total += length;
		lengths[i] = length;
	}

	return std::make_tuple(lengths, total);
}

Polygon Stroker::tangentQuadrangle(double r1, const Vec2D &k1, double r2, const Vec2D &k2, double d)
{
	double s = (r2 - r1) / d;
	double ss = s*s;

	if (ss >= 1.0)
		return Polygon();

	double c = sqrt(1.0 - ss);

	double ns = -s;
	double nc = -c;

	Vec2D k1_k2 = k2 - k1;
	Vec2D k1_k2_0 = k1_k2.extractX();
	Vec2D k1_k2_1 = k1_k2.extractY();

	Vec2D vp = Vec2D(ns, c) * k1_k2_0 + Vec2D(nc, ns) * k1_k2_1;
	Vec2D vq = Vec2D(ns, nc) * k1_k2_0 + Vec2D(c, ns) * k1_k2_1;

	Vec2D t = Vec2D(r1, r2) / d;
	Vec2D t1 = t.extractX();
	Vec2D t2 = t.extractY();

	Polygon poly(4);
	poly[0] = k1 + vq * t1;
	poly[1] = k2 + vq * t2;
	poly[2] = k2 + vp * t2;
	poly[3] = k1 + vp * t1;

	return poly;
}

void Stroker::drawInterval(const Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	double totalLength;
	QVector<double> lengths;
	std::tie(lengths, totalLength) = polygonLengths(polygon);
	drawInterval(polygon, lengths, totalLength, dataStart, dataEnd);
}

void Stroker::filterData(TabletInputData &data) const
{
	data.pressure *= data.pressure;
}

} // namespace PaintField
