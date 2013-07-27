#pragma once

#include <QPainterPath>
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "../curvesubdivision.h"
#include "../bitmap.h"
#include "../blendop.h"

namespace Malachite
{

class QPainterPath_vs
{
public:
	QPainterPath_vs(const QPainterPath &path) :
		_path(path),
		_index(0),
		_subdIndex(0),
	    _totalCount(0)
	{}
	
	void rewind(unsigned index) { Q_UNUSED(index); _index = 0; }
	unsigned vertex(double *x, double *y);
	
	int totalCount() { return _totalCount; }
	
private:
	const QPainterPath _path;
	int _index;
	Polygon _subdPolygon;
	int _subdIndex;
	int _totalCount;
};

template <class T_BaseRenderer>
class Renderer
{
public:
	Renderer(T_BaseRenderer &ren) :
		_ren(&ren)
	{}
	
	// from agg code
    template<class Scanline> 
    void render(const Scanline& sl)
    {
        int y = sl.y();
        unsigned num_spans = sl.num_spans();
        typename Scanline::const_iterator span = sl.begin();

        for(;;)
        {
            int x = span->x;
            if(span->len > 0)
            {
                //_ren->blendRasterizerSpan(x, y,span->len, span->covers);
				_ren->blendRasterizerSpan(x, y,span->len, Pointer<typename Scanline::cover_type>(span->covers, span->len * sizeof(typename Scanline::cover_type)));
            }
            else
            {
                _ren->blendRasterizerLine(x, y, -span->len, *(span->covers));
            }
            if(--num_spans == 0) break;
            ++span;
        }
    }
	
private:
	T_BaseRenderer *_ren;
};


template<class T_Rasterizer, class T_Scanline, class T_Renderer>
void renderScanlines(T_Rasterizer& ras, T_Scanline& sl, T_Renderer& ren)
{
    if(ras.rewind_scanlines())
    {
        sl.reset(ras.min_x(), ras.max_x());
        while(ras.sweep_scanline_f(sl))
        {
            ren.render(sl);
        }
    }
}

template <class T_Filler>
class ImageBaseRenderer8
{
public:
	ImageBaseRenderer8(const Bitmap<Pixel> &bitmap, BlendOp *blendOp, T_Filler *filler) :
		_bitmap(bitmap),
		_blendOp(blendOp),
		_filler(filler)
	{}
	
	void blendRasterizerSpan(int x, int y, int count, Pointer<const uint8_t> covers)
	{
		if (y < _bitmap.rect().top() || _bitmap.rect().bottom() < y)
			return;
		
		int start = qMax(x, _bitmap.rect().left());
		int end = qMin(x + count, _bitmap.rect().left() + _bitmap.rect().width());
		int newCount = end - start;
		
		if (newCount <= 0)
			return;
		
		Array<float> newCovers(newCount);
		
		for (int i = 0; i < newCount; ++i)
		{
			newCovers[i] = (float)covers[i + start - x] * (1.f / 255.f);
		}
		
		QPoint pos(start, y);
		_filler->fill(pos, newCount, _bitmap.pixelPointer(pos), newCovers.data(), _blendOp);
	}
	
	void blendRasterizerLine(int x, int y, int count, uint8_t cover)
	{
		if (y < _bitmap.rect().top() || _bitmap.rect().bottom() < y)
			return;
		
		int start = qMax(x, _bitmap.rect().left());
		int end = qMin(x + count, _bitmap.rect().left() + _bitmap.rect().width());
		int newCount = end - start;
		
		if (newCount <= 0)
			return;
		
		QPoint pos(start, y);
		_filler->fill(pos, newCount, _bitmap.pixelPointer(pos), float(cover) * (1.f / 255.f), _blendOp);
	}
	
private:
	Bitmap<Pixel> _bitmap;
	BlendOp *_blendOp;
	T_Filler *_filler;
};

template <class T_Filler>
class ImageBaseRenderer
{
public:
	ImageBaseRenderer(const Bitmap<Pixel> &bitmap, BlendOp *blendOp, float opacity, T_Filler *filler) :
		_bitmap(bitmap),
		_blendOp(blendOp),
		_opacity(opacity),
		_filler(filler)
	{}
	
	void blendRasterizerSpan(int x, int y, int count, Pointer<float> covers)
	{
		if (y < _bitmap.rect().top() || _bitmap.rect().bottom() < y)
			return;
		
		int start = qMax(x, _bitmap.rect().left());
		int end = qMin(x + count, _bitmap.rect().left() + _bitmap.rect().width());
		int newCount = end - start;
		
		if (newCount <= 0)
			return;
		
		if (_opacity != 1)
		{
			for (int i = 0; i < count; ++i)
				covers[i] *= _opacity;
		}
		
		QPoint pos(start, y);
		_filler->fill(pos, newCount, _bitmap.pixelPointer(pos), covers + (start - x), _blendOp);
	}
	
	void blendRasterizerLine(int x, int y, int count, float cover)
	{
		if (y < _bitmap.rect().top() || _bitmap.rect().bottom() < y)
			return;
		
		int start = qMax(x, _bitmap.rect().left());
		int end = qMin(x + count, _bitmap.rect().left() + _bitmap.rect().width());
		int newCount = end - start;
		
		if (newCount <= 0)
			return;
		
		cover *= _opacity;
		
		QPoint pos(start, y);
		
		if (cover == 1.f)
			_filler->fill(pos, newCount, _bitmap.pixelPointer(pos), _blendOp);
		else
			_filler->fill(pos, newCount, _bitmap.pixelPointer(pos), cover, _blendOp);
	}
	
private:
	Bitmap<Pixel> _bitmap;
	BlendOp *_blendOp;
	float _opacity;
	T_Filler *_filler;
};


}
