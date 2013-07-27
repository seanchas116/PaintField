#pragma once

//ExportName: BlendOp

#include "color.h"
#include "memory.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT BlendOp
{
public:
	
	enum Tile
	{
		NoTile = 0b00,
		TileDestination = 0b01,
		TileSource = 0b10,
		TileBoth = 0b11
	};
	
	Q_DECLARE_FLAGS(TileCombination, Tile)
	
	virtual void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const Pixel> masks) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const float> opacities) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, const Pixel &mask) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, float opacity) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, const Pixel &src) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, const Pixel &src, Pointer<const Pixel> masks) = 0;
	virtual void blend(int count, Pointer<Pixel> dst, const Pixel &src, Pointer<const float> opacities) = 0;
	
	virtual void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src) = 0;
	virtual void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const Pixel> masks) = 0;
	virtual void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const float> opacities) = 0;
	virtual void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, const Pixel &mask) = 0;
	virtual void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, float opacity) = 0;
	
	virtual TileCombination tileRequirement(TileCombination combination) = 0;
};

template <typename TBlendTraits>
class TemplateBlendOp : public BlendOp
{
public:
	void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, *src);
			++dst;
			++src;
		}
	}
	
	void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const Pixel> masks)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * masks->aV());
			++dst;
			++src;
			++masks;
		}
	}

	void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const float> opacities)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, *src * *opacities);
			++dst;
			++src;
			++opacities;
		}
	}

	void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, const Pixel &mask)
	{
		auto factor = mask.aV();
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * factor);
			++dst;
			++src;
		}
	}

	void blend(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, float opacity)
	{
		auto factor = PixelVec(opacity);
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * factor);
			++dst;
			++src;
		}
	}
	
	void blend(int count, Pointer<Pixel> dst, const Pixel &src)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src);
			++dst;
		}
	}
	
	void blend(int count, Pointer<Pixel> dst, const Pixel &src, Pointer<const Pixel> masks)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src.v() * masks->aV());
			++dst;
			++masks;
		}
	}
	
	void blend(int count, Pointer<Pixel> dst, const Pixel &src, Pointer<const float> opacities)
	{
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src * *opacities);
			++dst;
			++opacities;
		}
	}
	
	
	void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src)
	{
		src += count - 1;
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, *src);
			++dst;
			--src;
		}
	}
	
	void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const Pixel> masks)
	{
		src += count - 1;
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * masks->aV());
			++dst;
			--src;
			++masks;
		}
	}

	void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, Pointer<const float> opacities)
	{
		src += count - 1;
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, *src * *opacities);
			++dst;
			--src;
			++opacities;
		}
	}

	void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, const Pixel &mask)
	{
		src += count - 1;
		
		auto factor = mask.aV();
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * factor);
			++dst;
			--src;
		}
	}

	void blendReversed(int count, Pointer<Pixel> dst, Pointer<const Pixel> src, float opacity)
	{
		src += count - 1;
		
		auto factor = PixelVec(opacity);
		
		while (count--)
		{
			*dst = TBlendTraits::blend(*dst, src->v() * factor);
			++dst;
			--src;
		}
	}
	
	TileCombination tileRequirement(TileCombination combination)
	{
		return TBlendTraits::tileRequirement(combination);
	}
};

class MALACHITESHARED_EXPORT BlendOpDictionary
{
public:
	BlendOpDictionary();
	
	BlendOp *blendOp(int index) { return _blendOps.value(index, _defaultBlendOp); }
	
private:
	QHash<int, BlendOp *> _blendOps;
	BlendOp *_defaultBlendOp = 0;
};

MALACHITESHARED_EXPORT BlendOpDictionary *blendOpDictionary();

}

