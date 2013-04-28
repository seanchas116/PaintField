#include "pathrecorder.h"

namespace PaintField {

class PathRecorderPaintEngine : public QPaintEngine
{
public:
	
	PathRecorderPaintEngine();
	
	bool begin(QPaintDevice *pdev) override;
	void drawPath(const QPainterPath &path) override;
	void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) override;
	void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) override;
	bool end() override;
	Type type() const override;
	void updateState(const QPaintEngineState &state) override;
	
	QPainterPath path() const { return _path; }
	
private:
	
	QPainterPath _path;
	QTransform _transform;
};

PathRecorderPaintEngine::PathRecorderPaintEngine() :
	QPaintEngine(AllFeatures)
{
	
}

bool PathRecorderPaintEngine::begin(QPaintDevice *pdev)
{
	Q_UNUSED(pdev)
	return true;
}

void PathRecorderPaintEngine::drawPath(const QPainterPath &path)
{
	_path |= (path * _transform);
}

void PathRecorderPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
	QPolygonF polygon(pointCount);
	std::copy(points, points + pointCount, polygon.begin());
	
	QPainterPath path;
	path.addPolygon(polygon);
	
	if (mode == WindingMode)
	{
		path.setFillRule(Qt::WindingFill);
		path = path.simplified();
	}
	
	_path |= (path * _transform);
}

void PathRecorderPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
	Q_UNUSED(r)
	Q_UNUSED(pm)
	Q_UNUSED(sr)
}

bool PathRecorderPaintEngine::end()
{
	return true;
}

QPaintEngine::Type PathRecorderPaintEngine::type() const
{
	return User;
}

void PathRecorderPaintEngine::updateState(const QPaintEngineState &state)
{
	if (state.state() & QPaintEngine::DirtyTransform)
	{
		_transform = state.transform();
	}
}

struct PathRecorder::Data
{
	QScopedPointer<PathRecorderPaintEngine> engine;
};

PathRecorder::PathRecorder() :
	QPaintDevice(),
	d(new Data)
{
	d->engine.reset(new PathRecorderPaintEngine());
}

PathRecorder::~PathRecorder()
{
	delete d;
}

QPaintEngine *PathRecorder::paintEngine() const
{
	return d->engine.data();
}

int PathRecorder::metric(PaintDeviceMetric metric) const
{
	Q_UNUSED(metric)
	return 0;
}

QPainterPath PathRecorder::path() const
{
	return d->engine->path();
}



}
