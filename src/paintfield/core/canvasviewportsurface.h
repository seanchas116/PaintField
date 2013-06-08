#pragma once

#include <Malachite/Surface>

struct CanvasViewportTileTraits
{
	static constexpr int tileWidth() { return Malachite::Surface::tileWidth(); }
	static Malachite::ImageU8::PixelType defaultPixel() { return Malachite::ImageU8::PixelType(128, 128, 128, 255); }
};

typedef Malachite::GenericSurface<Malachite::ImageU8, CanvasViewportTileTraits> CanvasViewportSurface;
