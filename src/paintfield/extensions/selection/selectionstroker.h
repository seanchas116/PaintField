#pragma once

#include "paintfield/extensions/brushtool/stroker.h"

namespace PaintField {

class Selection;

class SelectionStroker : public Stroker
{
public:
	SelectionStroker(Selection *selection, bool isEraser);

protected:

	void drawFirst(const TabletInputData &data) override;
	void drawInterval(
		const Malachite::Polygon &polygon, const QVector<double> &polygonLengths, double totalLength,
		const TabletInputData &dataStart, const TabletInputData &dataEnd) override;

private:

	void drawPath(const QPainterPath &path);

	Selection *mSelection = 0;
	bool mEraser = false;
};

} // namespace PaintField
