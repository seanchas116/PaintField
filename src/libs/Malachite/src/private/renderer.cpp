#include "renderer.h"

namespace Malachite
{

unsigned QPainterPath_vs::vertex(double *x, double *y)
{
	forever
	{
		if (_subdIndex)
		{
			if (_subdIndex == _subdPolygon.size())
			{
				_subdIndex = 0;
				continue;
			}
			
			const Vec2D p = _subdPolygon.at(_subdIndex);
			*x = p.x();
			*y = p.y();
			_subdIndex++;
			
			_totalCount++;
			return agg::path_cmd_line_to;
		}
		
		if (_index == _path.elementCount())	// path終わり
			return agg::path_cmd_stop;
		
		const QPainterPath::Element element = _path.elementAt(_index);
		
		if (element.type == QPainterPath::CurveToElement)
		{
			QPainterPath::Element e1, e2, e3, e4;
			e1 = _path.elementAt(_index - 1);
			e2 = _path.elementAt(_index);
			e3 = _path.elementAt(_index + 1);
			e4 = _path.elementAt(_index + 2);
			
			_subdPolygon = CurveSubdivision(Vec2D(e1.x, e1.y), Vec2D(e2.x, e2.y), Vec2D(e3.x, e3.y), Vec2D(e4.x, e4.y)).polygon();
			_subdIndex = 1;
			_index += 3;
			continue;
		}
		
		_index++;
		*x = element.x;
		*y = element.y;
		
		_totalCount++;
		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			return agg::path_cmd_move_to;
		case QPainterPath::LineToElement:
		default:
			return agg::path_cmd_line_to;
		}
	}
}

}
