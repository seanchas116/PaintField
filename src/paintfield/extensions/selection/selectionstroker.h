#pragma once

#include "paintfield/extensions/brushtool/stroker.h"

namespace PaintField {

class Selection;

class SelectionStroker : public Stroker
{
public:
	SelectionStroker(Selection *selection);

protected:

	void drawFirst(const TabletInputData &data) override;
	void drawInterval(
		const Malachite::Polygon &polygon, const QVector<double> &polygonLengths, double totalLength,
		const TabletInputData &dataStart, const TabletInputData &dataEnd) override;

private:

	Selection *mSelection = 0;
};

} // namespace PaintField
