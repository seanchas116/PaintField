#pragma once

//ExportName: Image

#include "paintable.h"
#include "color.h"
#include "genericimage.h"
#include "pixel.h"
#include "blendmode.h"

namespace Malachite
{

class Image;

/**
 * Bgra 8bit-per-channel unsigned int Image
 * Compatible with QImage
 */
class MALACHITESHARED_EXPORT ImageU8 : public GenericImage<BgraPremultU8>
{
public:
	
	typedef GenericImage<BgraPremultU8> super;
	
	ImageU8() : super() {}
	
	ImageU8(const super &other) : super(other) {}
	
	ImageU8(const QSize &size) : super(size) {}
	
	ImageU8(int w, int h) : super(w, h) {}
	
	/**
	 * Wraps this into a 32bit premultiplied QImage.
	 * Do not delete the original image until the returned image is detached.
	 * @return 
	 */
	QImage wrapInQImage() const;
	
	/**
	 * Wraps a QImage.
	 * The QImage must be 32bit premultiplied.
	 * Do not delete the original image until the returned image is detached.
	 * @param image
	 * @return 
	 */
	static ImageU8 wrapQImage(const QImage &image);
	
	template <ImagePasteInversionMode InversionMode = ImagePasteNotInverted>
	void paste(const ImageU8 &image, const QPoint &point = QPoint())
	{
		QRect r = rect() & QRect(point, image.size());
		
		for (int y = r.top(); y <= r.bottom(); ++y)
		{
			PixelType *dp;
			
			if (InversionMode & ImagePasteDestinationInverted)
				dp = invertedScanline(y);
			else
				dp = scanline(y);
			
			dp += r.left();
			
			const PixelType *sp;
			
			if (InversionMode & ImagePasteSourceInverted)
				sp = image.invertedConstScanline(y - point.y());
			else
				sp = image.constScanline(y - point.y());
			
			sp += (r.left() - point.x());
			
			memcpy(dp, sp, r.width() * sizeof(PixelType));
		}
	}
};

/**
 * Bgra 32bit-per-channel float Image
 */
class MALACHITESHARED_EXPORT Image : public GenericImage<Pixel>, public Paintable
{
public:
	
	typedef GenericImage<Pixel> super;
	
	/**
	 * Constructs an empty image.
	 */
	Image() : super() {}
	
	/**
	 * Constructs a copy.
	 * @param other The original image
	 */
	Image(const super &other) : super(other) {}
	
	/**
	 * Constructs a image with a size.
	 * The data will not be initialized.
	 * @param size The size
	 */
	Image(const QSize &size) : super(size) {}
	
	/**
	 * Constructs a image with a size.
	 * The data will not be initialized.
	 * @param width The width
	 * @param height The height
	 */
	Image(int width, int height) : super(width, height) {}
	
	/**
	 *
	 * @return Whether the image is blank (all pixels are transparent)
	 */
	bool isBlank() const;
	
	/**
	 * Creates a PaintEngine. Used by Painter.
	 * @return a PaintEngine
	 */
	PaintEngine *createPaintEngine();
	
	void pasteWithBlendMode(BlendMode mode, float opacity, const Image &other, const QPoint &offset, const QRect &imageRect);
	
	/**
	 * Fills the background with white and returns the result.
	 * Use when an opaque version of an image is needed (eg JPEG save).
	 * @return The result
	 */
	Image toOpaqueImage() const;
	
	ImageU8 toImageU8() const;
	
	/**
	 * Multiplies each component an value.
	 * @param factor The factor
	 * @return 
	 */
	Image &operator*=(float factor);
};

inline Image operator*(const Image &image, float factor)
{
	Image result = image;
	result *= factor;
	return result;
}

MALACHITESHARED_EXPORT QDataStream &operator<<(QDataStream &out, const Image &image);
MALACHITESHARED_EXPORT QDataStream &operator>>(QDataStream &in, Image &image);

}

Q_DECLARE_METATYPE(Malachite::Image)

