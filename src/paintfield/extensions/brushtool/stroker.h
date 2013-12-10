#pragma once

#include "paintfield/core/tabletinputdata.h"
#include <Malachite/Polygon>
#include <Malachite/Surface>

namespace PaintField {

class Stroker
{
public:
	Stroker();
	virtual ~Stroker() {}

	void setRadiusBase(double radius) { mRadiusBase = radius; }
	double radiusBase() const { return mRadiusBase; }

	void setSmoothed(bool enabled) { mSmoothed = enabled; }
	bool isSmoothed() const { return mSmoothed; }

	void moveTo(const TabletInputData &data);
	void lineTo(const TabletInputData &data);
	void end();

	QHash<QPoint, QRect> lastEditedKeysWithRects() const { return mLastEditedKeysWithRects; }
	QPointSet totalEditedKeys() const { return mTotalEditedKeys; }

	void clearLastEditedKeys() { mLastEditedKeysWithRects.clear(); }

	static std::tuple<QVector<double>, double> polygonLengths(const Malachite::Polygon &polygon);

	/**
	 * @param distance Distance between center1 and center2
	 * @return
	 */
	static Malachite::Polygon tangentQuadrangle(double radius1, const Malachite::Vec2D &center1, double radius2, const Malachite::Vec2D &center2, double distance);

protected:
	virtual void drawFirst(const TabletInputData &data) = 0;
	virtual void drawInterval(const Malachite::Polygon &polygon, const QVector<double> &polygonLengths, double totalLength, const TabletInputData &dataStart, const TabletInputData &dataEnd) = 0;

	void addEditedKeys(const QHash<QPoint, QRect> &keysWithRects);
	void addEditedKey(const QPoint &key, const QRect &rect);
	void addEditedRect(const QRect &rect);

private:

	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	void filterData(TabletInputData &data) const;

	QPointSet mTotalEditedKeys;
	QHash<QPoint, QRect> mLastEditedKeysWithRects;

	int mCount = 0;
	TabletInputData  mDataPrev, mDataStart, mDataEnd;

	double mRadiusBase = 10;
	bool mSmoothed = false;

	Malachite::Polygon mSegment;
};

} // namespace PaintField
