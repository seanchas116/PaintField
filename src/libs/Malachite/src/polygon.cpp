#include <QDebug>

#include "private/agg_basics.h"
#include "curvesubdivision.h"
#include "memory.h"

#include "polygon.h"

using namespace Malachite;

// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)

namespace agg
{
//----------------------------------------------------------------ellipse
class ellipse
{
public:

	ellipse(const Vec2D &center, const Vec2D &radius) :
		m_center(center),
		m_radius(radius),
		m_scale(1.0),
		m_step(0)
	{
		calc_num_steps();
	}

    void approximation_scale(double scale);
    void rewind(unsigned path_id);
	unsigned vertex(Vec2D *vec);
	unsigned count() const { return m_num; }

private:
    void calc_num_steps();

	Vec2D m_center;
	Vec2D m_radius;
    double m_scale;
    unsigned m_num;
    unsigned m_step;
	double m_num_inv_2pi;
};

//------------------------------------------------------------------------
inline void ellipse::approximation_scale(double scale)
{   
    m_scale = scale;
    calc_num_steps();
}

//------------------------------------------------------------------------
inline void ellipse::calc_num_steps()
{
	double ra = (fabs(m_radius.x()) + fabs(m_radius.y())) / 2;
    double da = acos(ra / (ra + 0.125 / m_scale)) * 2;
    m_num = uround(2.0 * M_PI / da);
	m_num_inv_2pi = 1.0 / double(m_num) * 2.0 * M_PI;
}

//------------------------------------------------------------------------
inline void ellipse::rewind(unsigned)
{
    m_step = 0;
}

inline unsigned ellipse::vertex(Vec2D *vec)
{
	if(m_step == m_num) 
    {
        ++m_step;
        return path_cmd_end_poly | path_flags_close | path_flags_ccw;
    }
    if(m_step > m_num) return path_cmd_stop;
    double angle = double(m_step) * m_num_inv_2pi;
	
	Vec2D r(cos(angle), sin(angle));
	*vec = m_center + r * m_radius;
	
    m_step++;
    return ((m_step == 1) ? path_cmd_move_to : path_cmd_line_to);
}

}

namespace Malachite
{

Polygon::Polygon(const QVector<QPointF> &points) :
	QVector<Vec2D>(points.size())
{
	memcpy(data(), points.constData(), points.size() * sizeof(double) * 2);
}

Polygon &Polygon::operator*=(const QTransform &transform)
{
	for (Polygon::iterator i = begin(); i != end(); ++i)
		*i *= transform;
	
	return *this;
}

QPolygonF Polygon::toQPolygonF() const
{
	QPolygonF polygon(size());
	memcpy(polygon.data(), constData(), size() * sizeof(double) * 2);
	return polygon;
}

Polygon Polygon::fromRect(const QRectF &rect)
{
	Polygon polygon(4);
	polygon[0] = Vec2D(rect.x(), rect.y());
	polygon[1] = Vec2D(rect.x() + rect.width(), rect.y());
	polygon[2] = Vec2D(rect.x() + rect.width(), rect.y() + rect.height());
	polygon[3] = Vec2D(rect.x(), rect.y() + rect.height());
	return polygon;
}

Polygon Polygon::fromEllipse(const Vec2D &center, const Vec2D &radius)
{
	agg::ellipse ellipse(center, radius);
	
	Polygon poly(ellipse.count());
	
	for (auto iter = poly.begin(); iter != poly.end(); ++iter)
		ellipse.vertex(iter);
	
	return poly;
}

MultiPolygon MultiPolygon::fromQPainterPath(const QPainterPath &path)
{
	int count = path.elementCount();
	
	Polygon polygon;
	MultiPolygon polygons;
	
	int i = 0;
	while (i < count)
	{
		const QPainterPath::Element elem = path.elementAt(i);
		
		switch (elem.type)
		{
			case QPainterPath::CurveToElement:
			{
				QPainterPath::Element e1, e2, e3, e4;
				e1 = path.elementAt(i - 1);
				e2 = elem;
				e3 = path.elementAt(i + 1);
				e4 = path.elementAt(i + 2);
				
				polygon << CurveSubdivision(Vec2D(e1.x, e1.y), Vec2D(e2.x, e2.y), Vec2D(e3.x, e3.y), Vec2D(e4.x, e4.y)).polygon().mid(1);
				i += 3;
				break;
			}
			case QPainterPath::MoveToElement:
			{
				if (polygon.size() > 2)
					polygons << polygon;
				polygon = Polygon();
			}	// fall through
			case QPainterPath::LineToElement:
			{
				polygon << Vec2D(elem.x, elem.y);
			}	// fall through
			default:
			{
				++i;
				break;
			}
		}
	}
	
	if (polygon.size() > 2)
		polygons << polygon;
	
	return polygons;
}

MultiPolygon &MultiPolygon::operator*=(const QTransform &transform)
{
	for (MultiPolygon::iterator i = begin(); i != end(); ++i)
		*i *= transform;
	
	return *this;
}

}

