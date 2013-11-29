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

using ImageU8 = GenericImage<BgraPremultU8>;
using ConstImageU8 = GenericImage<const BgraPremultU8>;

/**
 * Wraps the image into a 32bit premultiplied QImage.
 * Do not delete the original image until the returned image is detached.
 */
QImage wrapInQImage(const ImageU8 &image);

/**
 * Wraps a QImage.
 * The QImage must be 32bit premultiplied.
 * Do not delete the original image until the returned image is detached.
 * @param image
 * @return
 */
ConstImageU8 wrapQImage(const QImage &image);

ImageU8 wrapQImage(QImage &image);

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

