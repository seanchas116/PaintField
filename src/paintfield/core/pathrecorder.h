#pragma once
#include "global.h"
#include <QPaintEngine>
#include <QPaintDevice>

namespace PaintField {

class PathRecorder : public QPaintDevice
{
public:
	PathRecorder();
	~PathRecorder();
	PathRecorder(const PathRecorder &other) = delete;
	
	QPaintEngine *paintEngine() const override;
	int metric(PaintDeviceMetric metric) const override;
	
	QPainterPath path() const;
	
private:
	
	struct Data;
	Data *d;
};

}
