#pragma once

#include "../vec2d.h"
#include "../blendop.h"
#include "../division.h"
#include "../interval.h"

namespace Malachite
{

class ColorFiller
{
public:
	ColorFiller(const Pixel &argb) :
		_argb(argb)
	{}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, Pointer<float> covers, BlendOp *blendOp)
	{
		Q_UNUSED(pos);
		blendOp->blend(count, dst, _argb, covers);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, float cover, BlendOp *blendOp)
	{
		Q_UNUSED(pos);
		blendOp->blend(count, dst, _argb * cover);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, BlendOp *blendOp)
	{
		Q_UNUSED(pos);
		blendOp->blend(count, dst, _argb);
	}
	
private:
	Pixel _argb;
};

template <Malachite::SpreadType T_SpreadType>
class ImageFiller;

template <>
class ImageFiller<Malachite::SpreadTypePad>
{
public:
	
	ImageFiller(const Bitmap<Pixel> &bitmap, const QPoint &offset) :
		_srcBitmap(bitmap), _offset(offset) {}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, Pointer<float> covers, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		srcPos.ry() = qBound(0, srcPos.y(), _srcBitmap.height() - 1);
		
		int srcX = srcPos.x();
		int srcY = srcPos.y();
		
		Interval dstInterval(pos.x(), count);
		Interval srcInterval(_offset.x(), _srcBitmap.width());
		
		auto interval = srcInterval & dstInterval;
		auto intervalLeftRight = dstInterval.subtracted(srcInterval);
		auto intervalLeft = intervalLeftRight[0];
		auto intervalRight = intervalLeftRight[1];
		
		int i = 0;
		
		if (intervalLeft.isValid())
		{
			blendOp->blend
			(
				intervalLeft.length(),
				dst,
				_srcBitmap.pixel(0, srcY),
				covers
			);
			
			i += intervalLeft.length();
		}
		
		if (interval.isValid())
		{
			blendOp->blend
			(
				interval.length(),
				dst + i,
				_srcBitmap.constPixelPointer(srcX + i, srcY),
				covers + i
			);
			
			i += interval.length();
		}
		
		if (intervalRight.isValid())
		{
			blendOp->blend
			(
				intervalRight.length(),
				dst + i,
				_srcBitmap.pixel(_srcBitmap.width() - 1, srcY),
				covers + i
			);
		}
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, float cover, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		srcPos.ry() = qBound(0, srcPos.y(), _srcBitmap.height() - 1);
		
		int srcX = srcPos.x();
		int srcY = srcPos.y();
		
		Interval dstInterval(pos.x(), count);
		Interval srcInterval(_offset.x(), _srcBitmap.width());
		
		auto interval = srcInterval & dstInterval;
		auto intervalLeftRight = dstInterval.subtracted(srcInterval);
		auto intervalLeft = intervalLeftRight[0];
		auto intervalRight = intervalLeftRight[1];
		
		int i = 0;
		
		if (intervalLeft.isValid())
		{
			blendOp->blend
			(
				intervalLeft.length(),
				dst,
				_srcBitmap.pixel(0, srcY) * cover
			);
			
			i += intervalLeft.length();
		}
		
		if (interval.isValid())
		{
			blendOp->blend
			(
				interval.length(),
				dst + i,
				_srcBitmap.constPixelPointer(srcX + i, srcY),
				cover
			);
			
			i += interval.length();
		}
		
		if (intervalRight.isValid())
		{
			blendOp->blend
			(
				intervalRight.length(),
				dst + i,
				_srcBitmap.pixel(_srcBitmap.width() - 1, srcY) * cover
			);
		}
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		srcPos.ry() = qBound(0, srcPos.y(), _srcBitmap.height() - 1);
		
		int srcX = srcPos.x();
		int srcY = srcPos.y();
		
		Interval dstInterval(pos.x(), count);
		Interval srcInterval(_offset.x(), _srcBitmap.width());
		
		auto interval = srcInterval & dstInterval;
		auto intervalLeftRight = dstInterval.subtracted(srcInterval);
		auto intervalLeft = intervalLeftRight[0];
		auto intervalRight = intervalLeftRight[1];
		
		int i = 0;
		
		if (intervalLeft.isValid())
		{
			blendOp->blend
			(
				intervalLeft.length(),
				dst,
				_srcBitmap.pixel(0, srcY)
			);
			
			i += intervalLeft.length();
		}
		
		if (interval.isValid())
		{
			blendOp->blend
			(
				interval.length(),
				dst + i,
				_srcBitmap.constPixelPointer(srcX + i, srcY)
			);
			
			i += interval.length();
		}
		
		if (intervalRight.isValid())
		{
			blendOp->blend
			(
				intervalRight.length(),
				dst + i,
				_srcBitmap.pixel(_srcBitmap.width() - 1, srcY)
			);
		}
	}
	
	
private:
	
	const Bitmap<Pixel> _srcBitmap;
	QPoint _offset;
};

template <>
class ImageFiller<Malachite::SpreadTypeRepeat>
{
public:
	
	ImageFiller(const Bitmap<Pixel> &bitmap, const QPoint &offset) :
		_srcBitmap(bitmap), _offset(offset) {}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, Pointer<float> covers, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(), _srcBitmap.width());
		IntDivision divY(srcPos.y(), _srcBitmap.height());
		
		int imageY = divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		if (i >= count)
		{
			blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), covers);
			return;
		}
		
		blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), covers);
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), covers + i);
			
			i += _srcBitmap.width();
		}
		
		blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY), covers + i);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, float cover, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(), _srcBitmap.width());
		IntDivision divY(srcPos.y(), _srcBitmap.height());
		
		int imageY = divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		if (i >= count)
		{
			blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), cover);
			return;
		}
		
		blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), cover);
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), cover);
			
			i += _srcBitmap.width();
		}
		
		blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY), cover);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(), _srcBitmap.width());
		IntDivision divY(srcPos.y(), _srcBitmap.height());
		
		int imageY = divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		if (i >= count)
		{
			blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY));
			return;
		}
		
		blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY));
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY));
			
			i += _srcBitmap.width();
		}
		
		blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY));
	}
	
private:
	
	const Bitmap<Pixel> _srcBitmap;
	QPoint _offset;
};

template <>
class ImageFiller<Malachite::SpreadTypeReflective>
{
public:
	
	ImageFiller(const Bitmap<Pixel> &bitmap, const QPoint &offset) :
		_srcBitmap(bitmap), _offset(offset) {}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, Pointer<float> covers, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(),  _srcBitmap.width());
		IntDivision divY(srcPos.y(),  _srcBitmap.height());
		
		int imageY = divY.quot() % 2 ?  _srcBitmap.height() - divY.rem() - 1 : divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		int q = divX.quot();
		
		if (i >= count)
		{
			if (q % 2)
				blendOp->blendReversed(count, dst, _srcBitmap.constPixelPointer( _srcBitmap.width() - divX.rem() - count, imageY), covers);
			else
				blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), covers);
			
			return;
		}
		
		if (q % 2)
			blendOp->blendReversed(i, dst, _srcBitmap.constPixelPointer(0, imageY), covers);
		else
			blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), covers);
		
		q++;
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			if (q % 2)
				blendOp->blendReversed(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), covers + i);
			else
				blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), covers + i);
			
			i += _srcBitmap.width();
			q++;
		}
		
		if (q % 2)
			blendOp->blendReversed(count - i, dst + i, _srcBitmap.constPixelPointer(_srcBitmap.width() - count + i, imageY), covers + i);
		else
			blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY), covers + i);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, float cover, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(),  _srcBitmap.width());
		IntDivision divY(srcPos.y(),  _srcBitmap.height());
		
		int imageY = divY.quot() % 2 ?  _srcBitmap.height() - divY.rem() - 1 : divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		int q = divX.quot();
		
		if (i >= count)
		{
			if (q % 2)
				blendOp->blendReversed(count, dst, _srcBitmap.constPixelPointer( _srcBitmap.width() - divX.rem() - count, imageY), cover);
			else
				blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), cover);
			
			return;
		}
		
		if (q % 2)
			blendOp->blendReversed(i, dst, _srcBitmap.constPixelPointer(0, imageY), cover);
		else
			blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY), cover);
		
		q++;
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			if (q % 2)
				blendOp->blendReversed(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), cover);
			else
				blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY), cover);
			
			i += _srcBitmap.width();
			q++;
		}
		
		if (q % 2)
			blendOp->blendReversed(count - i, dst + i, _srcBitmap.constPixelPointer(_srcBitmap.width() - count + i, imageY), cover);
		else
			blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY), cover);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, BlendOp *blendOp)
	{
		QPoint srcPos = pos - _offset;
		IntDivision divX(srcPos.x(),  _srcBitmap.width());
		IntDivision divY(srcPos.y(),  _srcBitmap.height());
		
		int imageY = divY.quot() % 2 ?  _srcBitmap.height() - divY.rem() - 1 : divY.rem();
		
		int i = _srcBitmap.width() - divX.rem();
		
		int q = divX.quot();
		
		if (i >= count)
		{
			if (q % 2)
				blendOp->blendReversed(count, dst, _srcBitmap.constPixelPointer( _srcBitmap.width() - divX.rem() - count, imageY));
			else
				blendOp->blend(count, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY));
			
			return;
		}
		
		if (q % 2)
			blendOp->blendReversed(i, dst, _srcBitmap.constPixelPointer(0, imageY));
		else
			blendOp->blend(i, dst, _srcBitmap.constPixelPointer(divX.rem(), imageY));
		
		q++;
		
		forever
		{
			if (count - i < _srcBitmap.width()) break;
			
			if (q % 2)
				blendOp->blendReversed(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY));
			else
				blendOp->blend(_srcBitmap.width(), dst + i, _srcBitmap.constPixelPointer(0, imageY));
			
			i += _srcBitmap.width();
			q++;
		}
		
		if (q % 2)
			blendOp->blendReversed(count - i, dst + i, _srcBitmap.constPixelPointer(_srcBitmap.width() - count + i, imageY));
		else
			blendOp->blend(count - i, dst + i, _srcBitmap.constPixelPointer(0, imageY));
	}
	
private:
	
	const Bitmap<Pixel> _srcBitmap;
	QPoint _offset;
};

template <class T_Generator, bool TransformEnabled>
class Filler
{
public:
	Filler(T_Generator *generator, const QTransform &worldTransform = QTransform()) :
		_generator(generator),
		_transform(worldTransform)
	{}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, Pointer<float> covers, BlendOp *blendOp)
	{
		Array<Pixel> fill(count);
		
		Vec2D centerPos(pos.x(), pos.y());
		centerPos += Vec2D(0.5, 0.5);
		
		for (int i = 0; i < count; ++i)
		{
			fill[i] = _generator->at(TransformEnabled ? centerPos * _transform : centerPos);
			centerPos += Vec2D(1, 0);
		}
		
		blendOp->blend(count, dst, fill.data(), covers);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, float cover, BlendOp *blendOp)
	{
		Array<Pixel> fill(count);
		
		Vec2D centerPos(pos.x(), pos.y());
		centerPos += Vec2D(0.5, 0.5);
		
		for (int i = 0; i < count; ++i)
		{
			fill[i] = _generator->at(TransformEnabled ? centerPos * _transform : centerPos);
			centerPos += Vec2D(1, 0);
		}
		
		blendOp->blend(count, dst, fill.data(), cover);
	}
	
	void fill(const QPoint &pos, int count, Pointer<Pixel> dst, BlendOp *blendOp)
	{
		Array<Pixel> fill(count);
		
		Vec2D centerPos(pos.x(), pos.y());
		centerPos += Vec2D(0.5, 0.5);
		
		for (int i = 0; i < count; ++i)
		{
			fill[i] = _generator->at(TransformEnabled ? centerPos * _transform : centerPos);
			centerPos += Vec2D(1, 0);
		}
		
		blendOp->blend(count, dst, fill.data());
	}
	
private:
	T_Generator *_generator;
	QTransform _transform;
};

}

