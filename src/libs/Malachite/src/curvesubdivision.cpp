// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)

#include "private/agg_array.h"
#include "private/agg_math.h"
#include "curvesubdivision.h"

namespace agg
{

using namespace Malachite;

//-------------------------------------------------------------curve4_inc
class curve4_inc
{
public:
    curve4_inc() :
        m_scale(1.0) { }

    curve4_inc(const Vec2D &p1, const Vec2D &p2, const Vec2D &p3, const Vec2D &p4) :
        m_scale(1.0) 
    { 
		init(p1, p2, p3, p4);
    }

	void init(const Vec2D &p1, const Vec2D &p2, const Vec2D &p3, const Vec2D &p4);

	void approximation_scale(double s) { m_scale = s; }
	double approximation_scale() const { return m_scale; }
	
	Polygon polygon() const { return m_points; }
	
private:
    
    double   m_scale;
	Polygon m_points;
};

//------------------------------------------------------------------------
void curve4_inc::init(const Vec2D &p1, const Vec2D &p2, const Vec2D &p3, const Vec2D &p4)
{
	auto d1 = p2 - p1;
	auto d2 = p3 - p2;
	auto d3 = p4 - p3;
	
	double len = d1.length() + d2.length() + d3.length() * 0.25 * m_scale;

    auto num_steps = std::round(len);

    if(num_steps < 4)
    {
        num_steps = 4;
    }

    double subdivide_step  = 1.0 / num_steps;
    double subdivide_step2 = subdivide_step * subdivide_step;
    double subdivide_step3 = subdivide_step2 * subdivide_step;

    double pre1 = 3.0 * subdivide_step;
    double pre2 = 3.0 * subdivide_step2;
    double pre4 = 6.0 * subdivide_step2;
    double pre5 = 6.0 * subdivide_step3;
	
	auto tmp1 = p1 - p2 * 2.0 + p3;
	auto tmp2 = (p2 - p3) * 3.0 - p1 + p4;

	auto f = p1;
	auto df = (p2 - p1) * pre1 + tmp1 * pre2 + tmp2 * subdivide_step3;
	auto ddf = tmp1 * pre4 + tmp2 * pre5;
	
	auto dddf = tmp2 * pre5;
	
	m_points.resize(num_steps);
	
	m_points[0] = p1;
	
	for (int i = 1; i < num_steps - 1; ++i)
	{
		f += df;
		df += ddf;
		ddf += dddf;
		m_points[i] = f;
	}
	
	m_points[num_steps - 1] = p4;
}

//-------------------------------------------------------------curve4_div
class curve4_div
{
public:
    curve4_div() : 
        m_approximation_scale(1.0),
        m_angle_tolerance(0.0),
        m_cusp_limit(0.0)
    {}

    curve4_div(const Vec2D &p1,
               const Vec2D &p2,
               const Vec2D &p3,
               const Vec2D &p4) :
        m_approximation_scale(1.0),
        m_angle_tolerance(0.0),
        m_cusp_limit(0.0)
    { 
        init(p1, p2, p3, p4);
    }

    void reset() { m_points.clear(); }
    void init(const Vec2D &p1,
			  const Vec2D &p2,
              const Vec2D &p3,
              const Vec2D &p4);

    void approximation_scale(double s) { m_approximation_scale = s; }
    double approximation_scale() const { return m_approximation_scale;  }

    void angle_tolerance(double a) { m_angle_tolerance = a; }
    double angle_tolerance() const { return m_angle_tolerance;  }

    void cusp_limit(double v) 
    { 
        m_cusp_limit = (v == 0.0) ? 0.0 : pi - v; 
    }

    double cusp_limit() const 
    { 
        return (m_cusp_limit == 0.0) ? 0.0 : pi - m_cusp_limit; 
    }

	Polygon polygon() const { return m_points; }
	
	int size() const
	{
		return m_points.size();
	}

private:
    void bezier(const Vec2D &p1,
				const Vec2D &p2,
                const Vec2D &p3,
                const Vec2D &p4);

    void recursive_bezier(const Vec2D &p1,
						  const Vec2D &p2,
			              const Vec2D &p3,
			              const Vec2D &p4,
                          unsigned level);

    double               m_approximation_scale;
    double               m_distance_tolerance_square;
    double               m_angle_tolerance;
    double               m_cusp_limit;
    Polygon m_points;
};

//------------------------------------------------------------------------
const double curve_distance_epsilon                  = 1e-30;
const double curve_collinearity_epsilon              = 1e-30;
const double curve_angle_tolerance_epsilon           = 0.01;
enum curve_recursion_limit_e { curve_recursion_limit = 32 };

//------------------------------------------------------------------------
void curve4_div::init(const Vec2D &p1, const Vec2D &p2, 
                      const Vec2D &p3, const Vec2D &p4)
{
    m_points.clear();
    m_distance_tolerance_square = 0.5 / m_approximation_scale;
    m_distance_tolerance_square *= m_distance_tolerance_square;
    bezier(p1, p2, p3, p4);
}

//------------------------------------------------------------------------
void curve4_div::recursive_bezier(const Vec2D &p1, const Vec2D &p2, 
                                  const Vec2D &p3, const Vec2D &p4, 
                                  unsigned level)
{
    if(level > curve_recursion_limit) 
    {
        return;
    }

    // Calculate all the mid-points of the line segments
    //----------------------
	
	Vec2D p12 = (p1 + p2) * 0.5;
	Vec2D p23 = (p2 + p3) * 0.5;
	Vec2D p34 = (p3 + p4) * 0.5;
	Vec2D p123 = (p12 + p23) * 0.5;
	Vec2D p234 = (p23 + p34) * 0.5;
	Vec2D p1234 = (p123 + p234) * 0.5;

    // Try to approximate the full cubic curve by a single straight line
    //------------------
	Vec2D d = p4 - p1;
	
	Vec2D d42 = p2 - p4;
	Vec2D d43 = p3 - p4;
	
	Vec2D d2d3 = Vec2D(d42.x(), d43.x()) * d.y() - Vec2D(d42.y(), d43.y()) * d.x();
	double d2 = fabs(d2d3.x());
	double d3 = fabs(d2d3.x());
	
	Vec2D da;
    double k;

    switch((int(d2 > curve_collinearity_epsilon) << 1) +
            int(d3 > curve_collinearity_epsilon))
    {
    case 0:
        // All collinear OR p1==p4
        //----------------------
		k = d.lengthSquare();
        if(k == 0)
        {
			d2 = (p2 - p1).lengthSquare();
			d3 = (p3 - p4).lengthSquare();
        }
        else
        {
            k   = 1 / k;
			da = p2 - p1;
			d2 = k * Vec2D::dot(da, d);
			da = p3 - p1;
			d3 = k * Vec2D::dot(da, d);
            if(d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
            {
                // Simple collinear case, 1---2---3---4
                // We can leave just two endpoints
                return;
            }
                 if(d2 <= 0) d2 = (p1 - p2).lengthSquare();
            else if(d2 >= 1) d2 = (p4 - p2).lengthSquare();
            else             d2 = (p1 + d2 * d - p2).lengthSquare();

                 if(d3 <= 0) d3 = (p1 - p3).lengthSquare();
            else if(d3 >= 1) d3 = (p4 - p3).lengthSquare();
            else             d3 = (p1 + d3 * d - p3).lengthSquare();
        }
        if(d2 > d3)
        {
            if(d2 < m_distance_tolerance_square)
            {
                m_points << p2;
                return;
            }
        }
        else
        {
            if(d3 < m_distance_tolerance_square)
            {
                m_points << p3;
                return;
            }
        }
        break;

    case 1:
        // p1,p2,p4 are collinear, p3 is significant
        //----------------------
        if(d3 * d3 <= m_distance_tolerance_square * d.lengthSquare())
        {
            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
            {
                m_points << p23;
                return;
            }

            // Angle Condition
            //----------------------
			
			Vec2D d34 = p4 - p3;
			Vec2D d23 = p3 - p2;
			da.rx() = fabs(atan2(d34.y(), d34.x()) - atan2(d23.y(), d23.x()));
			
			if(da.x() >= pi) da.rx() = 2*pi - da.x();

			if(da.x() < m_angle_tolerance)
            {
                m_points << p2;
                m_points << p3;
                return;
            }

            if(m_cusp_limit != 0.0)
            {
				if(da.x() > m_cusp_limit)
                {
                    m_points << p3;
                    return;
                }
            }
        }
        break;

    case 2:
        // p1,p3,p4 are collinear, p2 is significant
        //----------------------
        if(d2 * d2 <= m_distance_tolerance_square * d.lengthSquare())
        {
            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
            {
                m_points << p23;
                return;
            }

            // Angle Condition
            //----------------------
			
			Vec2D d23 = p3 - p2;
			Vec2D d12 = p2 - p1;
			da.rx() = fabs(atan2(d23.y(), d23.x()) - atan2(d12.y(), d12.x()));
			if(da.x() >= pi) da.rx() = 2*pi - da.x();

			if(da.x() < m_angle_tolerance)
            {
                m_points << p2;
                m_points << p3;
                return;
            }

            if(m_cusp_limit != 0.0)
            {
				if(da.x() > m_cusp_limit)
                {
                    m_points << p2;
                    return;
                }
            }
        }
        break;

    case 3: 
        // Regular case
        //-----------------
        if((d2 + d3)*(d2 + d3) <= m_distance_tolerance_square * d.lengthSquare())
        {
            // If the curvature doesn't exceed the distance_tolerance value
            // we tend to finish subdivisions.
            //----------------------
            if(m_angle_tolerance < curve_angle_tolerance_epsilon)
            {
                m_points << p23;
                return;
            }

            // Angle & Cusp Condition
            //----------------------
			
			Vec2D d34 = p4 - p3;
			Vec2D d23 = p3 - p2;
			Vec2D d12 = p2 - p1;
			
			
			k   = atan2(d23.y(), d23.x());
            da.rx() = fabs(k - atan2(d12.y(), d12.x()));
            da.ry() = fabs(atan2(d34.y(), d34.x()) - k);
            if(da.x() >= pi) da.rx() = 2*pi - da.x();
            if(da.y() >= pi) da.ry() = 2*pi - da.y();

            if(da.x() + da.y() < m_angle_tolerance)
            {
                // Finally we can stop the recursion
                //----------------------
                m_points << p23;
                return;
            }

            if(m_cusp_limit != 0.0)
            {
                if(da.x() > m_cusp_limit)
                {
                    m_points << p2;
                    return;
                }

                if(da.y() > m_cusp_limit)
                {
                    m_points << p3;
                    return;
                }
            }
        }
        break;
    }

    // Continue subdivision
    //----------------------
	recursive_bezier(p1, p12, p123, p1234, level + 1);
	recursive_bezier(p1234, p234, p34, p4, level + 1);
}

//------------------------------------------------------------------------
void curve4_div::bezier(const Vec2D &p1,
						const Vec2D &p2,
		                const Vec2D &p3,
		                const Vec2D &p4)
{
    m_points << p1;
    recursive_bezier(p1, p2, p3, p4, 0);
    m_points << p4;
}

}

namespace Malachite
{

CurveSubdivision::CurveSubdivision(const Curve4 &curve, Type type) :
    CurveSubdivision(curve.start, curve.control1, curve.control2, curve.end, type)
{}

CurveSubdivision::CurveSubdivision(const Vec2D &start, const Vec2D &control1, const Vec2D &control2, const Vec2D &end, Type type)
{
	if (type == TypeAdaptive)
	{
		agg::curve4_div curveSub(start, control1, control2, end);
		_polygon = curveSub.polygon();
	}
	else
	{
		agg::curve4_inc curveSub(start, control1, control2, end);
		_polygon = curveSub.polygon();
	}
}

}
