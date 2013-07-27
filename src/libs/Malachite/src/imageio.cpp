#include <QFile>
#include "imageio.h"

#include <FreeImage.h>

namespace Malachite
{

static unsigned readFromQIODevice(void *buffer, unsigned size, unsigned count, void *handle)
{
	auto ioDevice = static_cast<QIODevice *>(handle);
	return ioDevice->read(static_cast<char *>(buffer), size * count);
}

static unsigned writeToQIODevice(void *buffer, unsigned size, unsigned count, void *handle)
{
	auto ioDevice = static_cast<QIODevice *>(handle);
	return ioDevice->write(static_cast<const char *>(buffer), size * count);
}

static int seekQIODevice(void *handle, long offset, int origin)
{
	auto ioDevice = static_cast<QIODevice *>(handle);
	
	bool succeeded = false;
	
	switch (origin)
	{
		case SEEK_SET:
			succeeded = ioDevice->seek(offset);
			break;
		case SEEK_CUR:
			succeeded = ioDevice->seek(offset + ioDevice->pos());
			break;
		default:
			Q_ASSERT(0);
			break;
	}
	
	return succeeded ? 0 : -1;
}

static long tellQIODevice(void *handle)
{
	auto ioDevice = static_cast<QIODevice *>(handle);
	return ioDevice->pos();
}

static void outputMessage(FREE_IMAGE_FORMAT fif, const char *message)
{
	qWarning() << "FreeImage Output";
	
	if (fif != FIF_UNKNOWN)
		qWarning() << "In Format" << FreeImage_GetFormatFromFIF(fif);
	
	qWarning() << message;
}


template <class T_Image>
static bool pasteFIBITMAPToImage(const QPoint &pos, T_Image *dst, FIBITMAP *src)
{
	FREE_IMAGE_TYPE srcType = FreeImage_GetImageType(src);
	QSize srcSize(FreeImage_GetWidth(src), FreeImage_GetHeight(src));
	const uint8_t *srcBits = FreeImage_GetBits(src);
	int srcPitch = FreeImage_GetPitch(src);
	
	switch (srcType)
	{
		case FIT_BITMAP:
		{
			int bpp = FreeImage_GetBPP(src);
			
			switch (bpp)
			{
				case 24:
				{
					auto wrapped = GenericImage<BgrU8>::wrap(srcBits, srcSize, srcPitch);
					dst->template paste<ImagePasteSourceInverted>(wrapped, pos);
					break;
				}
				case 32:
				{
					dst->template paste<ImagePasteSourceInverted>(GenericImage<BgraU8>::wrap(srcBits, srcSize, srcPitch), pos);
					break;
				}
				default:
				{
					FIBITMAP *newBitmap = FreeImage_ConvertTo32Bits(src);	// converted to RGBA8
					dst->template paste<ImagePasteSourceInverted>(GenericImage<BgraU8>::wrap(FreeImage_GetBits(newBitmap), srcSize, FreeImage_GetPitch(newBitmap)), pos);
					FreeImage_Unload(newBitmap);
					break;
				}
			}
			
			break;
		}
		case FIT_RGB16:
		{
			dst->template paste<ImagePasteSourceInverted>(GenericImage<RgbaU16>::wrap(srcBits, srcSize, srcPitch), pos);
			break;
		}
		case FIT_RGBA16:
		{
			dst->template paste<ImagePasteSourceInverted>(GenericImage<RgbaU16>::wrap(srcBits, srcSize, srcPitch), pos);
			break;
		}
		default:
			qWarning() << Q_FUNC_INFO << ": Unsupported data type";
			return false;
	}
	
	return true;
}

template <class T_Image>
static bool pasteImageToFIBITMAP(const QPoint &pos, FIBITMAP *dst, const T_Image &src)
{
	FREE_IMAGE_TYPE dstType = FreeImage_GetImageType(dst);
	QSize dstSize(FreeImage_GetWidth(dst), FreeImage_GetHeight(dst));
	int dstPitch = FreeImage_GetPitch(dst);
	uint8_t *dstBits = FreeImage_GetBits(dst);
	
	switch (dstType)
	{
		case FIT_BITMAP:
		{
			int bpp = FreeImage_GetBPP(dst);
			
			switch (bpp)
			{
				case 24:
				{
					auto wrapper = GenericImage<BgrU8>::wrap(dstBits, dstSize, dstPitch);
					wrapper.paste<ImagePasteDestinationInverted>(src, pos);
					break;
				}
				case 32:
				{
					auto wrapper = GenericImage<BgraU8>::wrap(dstBits, dstSize, dstPitch);
					wrapper.paste<ImagePasteDestinationInverted>(src, pos);
					break;
				}
				default:
					qWarning() << Q_FUNC_INFO << ": Unsupported data type";
					return false;
			}
			
			break;
		}
		case FIT_RGB16:
		{
			auto wrapper = GenericImage<RgbU16>::wrap(dstBits, dstSize, dstPitch);
			wrapper.paste<ImagePasteDestinationInverted>(src, pos);
			break;
		}
		case FIT_RGBA16:
		{
			auto wrapper = GenericImage<RgbaU16>::wrap(dstBits, dstSize, dstPitch);
			wrapper.paste<ImagePasteDestinationInverted>(src, pos);
			break;
		}
		default:
			qWarning() << Q_FUNC_INFO << ": Unsupported data type";
			return false;
	}
	
	return true;
}


struct ImageImporter::Data
{
	FIBITMAP *bitmap = 0;
	QSize size;
	
	~Data()
	{
		deleteBitmap();
	}
	
	void deleteBitmap()
	{
		if (bitmap)
		{
			FreeImage_Unload(bitmap);
			bitmap = 0;
		}
	}
};

ImageImporter::ImageImporter() :
    d(new Data)
{}

ImageImporter::~ImageImporter()
{
	delete d;
}

bool ImageImporter::load(QIODevice *device)
{
	d->deleteBitmap();
	
	FreeImage_SetOutputMessage(outputMessage);
	
	FreeImageIO io;
	io.read_proc = readFromQIODevice;
	io.write_proc = 0;
	io.seek_proc = seekQIODevice;
	io.tell_proc = tellQIODevice;
	
	auto format = FreeImage_GetFileTypeFromHandle(&io, device);
	
	if (format != FIF_UNKNOWN)
	{
		int flags = 0;
		
		if (format == FIF_JPEG)
			flags = JPEG_ACCURATE;
		
		d->bitmap = FreeImage_LoadFromHandle(format, &io, device, flags);
	}
	
	if (d->bitmap)
	{
		int w = FreeImage_GetWidth(d->bitmap);
		int h = FreeImage_GetHeight(d->bitmap);
		
		d->size = QSize(w, h);
	}
	
	return d->bitmap;
}

bool ImageImporter::load(const QString &filepath)
{
	QFile file(filepath);
	
	if (!file.open(QIODevice::ReadOnly))
		return false;
	
	return load(&file);
}

bool ImageImporter::isValid() const
{
	return d->bitmap;
}

QSize ImageImporter::size() const
{
	return d->size;
}

Image ImageImporter::toImage() const
{
	if (!isValid())
		return Image();
	
	Image image(size());
	
	pasteFIBITMAPToImage(QPoint(), &image, d->bitmap);
	return image;
}

Surface ImageImporter::toSurface(const QPoint &p) const
{
	if (!isValid())
		return Surface();
	
	Surface surface;
	pasteFIBITMAPToImage(p, &surface, d->bitmap);
	return surface;
}

QStringList ImageImporter::importableExtensions()
{
	return { "bmp", "png", "jpg", "jpeg" };
}



struct ImageExporter::Data
{
	~Data()
	{
		deleteBitmap();
	}
	
	void setFormatString(const QString &formatString)
	{
		if (formatString == "bmp")
			format = FIF_BMP;
		else if (formatString == "jpg" || formatString == "jpeg")
			format = FIF_JPEG;
		else if (formatString == "png")
			format = FIF_PNG;
		else
			format = FIF_UNKNOWN;
	}
	
	void deleteBitmap()
	{
		if (bitmap)
		{
			FreeImage_Unload(bitmap);
			bitmap = 0;
		}
	}
	
	void allocate(const QSize &size)
	{
		deleteBitmap();
		
		switch (format)
		{
			case FIF_PNG:
				if (alphaEnabled)
					bitmap = FreeImage_AllocateT(FIT_RGBA16, size.width(), size.height());
				else
					bitmap = FreeImage_AllocateT(FIT_RGB16, size.width(), size.height());
				break;
			default:
				bitmap = FreeImage_Allocate(size.width(), size.height(), 24);
		}
	}
	
	QSize size;
	FIBITMAP *bitmap = 0;
	FREE_IMAGE_FORMAT format;
	int quality = 80;
	bool alphaEnabled = true;
};

ImageExporter::ImageExporter(const QString &format, bool alphaEnabled) :
    d(new Data)
{
	d->alphaEnabled = alphaEnabled;
	d->setFormatString(format);
}

ImageExporter::~ImageExporter()
{
	delete d;
}

void ImageExporter::setQuality(int quality)
{
	d->quality = quality;
}

int ImageExporter::quality() const
{
	return d->quality;
}

bool ImageExporter::isAlphaEnabled() const
{
	return d->alphaEnabled;
}

bool ImageExporter::save(QIODevice *device)
{
	if (!d->bitmap)
		return false;
	
	if (d->format == FIF_UNKNOWN)
		return false;
	
	int flags = 0;
	
	if (d->format == FIF_JPEG)
		flags = d->quality;
	
	FreeImage_SetOutputMessage(outputMessage);
	
	FreeImageIO io;
	io.read_proc = 0;
	io.write_proc = writeToQIODevice;
	io.seek_proc = seekQIODevice;
	io.tell_proc = tellQIODevice;
	
	return FreeImage_SaveToHandle(d->format, d->bitmap, &io, device, flags);
}

bool ImageExporter::save(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	
	return save(&file);
}

bool ImageExporter::setImage(const Image &image)
{
	d->allocate(image.size());
	if (!d->bitmap)
		return false;
	
	return pasteImage(image, QPoint());
}

bool ImageExporter::setSurface(const Surface &surface, const QRect &rect)
{
	auto size = rect.size();
	
	d->allocate(size);
	if (!d->bitmap)
		return false;
	
	auto pos = rect.topLeft();
	
	for (const QPoint &key : Surface::rectToKeys(QRect(pos, size)))
	{
		if (pasteImage(surface.tile(key), key * Surface::tileWidth() + pos) == false)
			return false;
	}
	
	return true;
}

bool ImageExporter::pasteImage(const Image &image, const QPoint &pos)
{
	if (!d->bitmap)
		return false;
	
	return pasteImageToFIBITMAP(pos, d->bitmap, image);
}


}

