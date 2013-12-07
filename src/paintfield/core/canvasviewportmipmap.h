#pragma once

#include <Malachite/SurfaceMipmap>
#include "canvasviewportsurface.h"

namespace PaintField {

struct CanvasViewportMipmapPixelTraits
{
	static Malachite::BgraPremultU8 average(const std::array<Malachite::BgraPremultU8, 4> &pixels)
	{
		int r = 0;
		int g = 0;
		int b = 0;
		for (const auto &pixel : pixels) {
			r += int(pixel.r());
			g += int(pixel.g());
			b += int(pixel.b());
		}
		r /= 4;
		g /= 4;
		b /= 4;
		Malachite::BgraPremultU8 result;
		result.setA(255);
		result.setR(r);
		result.setG(g);
		result.setB(b);
		return result;
	}
};

using CanvasViewportMipmap = Malachite::SurfaceMipmap<CanvasViewportSurface, CanvasViewportMipmapPixelTraits>;

} // namespace PaintField
