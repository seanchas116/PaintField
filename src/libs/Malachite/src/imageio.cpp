#include "imageio.h"
#include <QFile>
#include <boost/utility.hpp>
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
	
	switch (origin) {
		case SEEK_SET:
			succeeded = ioDevice->seek(offset);
			break;
		case SEEK_CUR:
			succeeded = ioDevice->seek(offset + ioDevice->pos());
			break;
		case SEEK_END:
			succeeded = ioDevice->seek(offset + ioDevice->size());
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

struct FIBITMAPDeleter
{
	void operator()(FIBITMAP *bitmap) const
	{
		FreeImage_Unload(bitmap);
	}
};

using FIBITMAPUniquePtr = std::unique_ptr<FIBITMAP, FIBITMAPDeleter>;

template <class TPixel>
class FIBITMAPWrapper :
	public ImageSizeAccessible<FIBITMAPWrapper<TPixel>>,
	public ImagePixelsAccessible<FIBITMAPWrapper<TPixel>, TPixel *, const TPixel *>
{
public:

	using value_type = TPixel;
	using iterator = TPixel *;
	using const_iterator = const TPixel *;

	FIBITMAPWrapper(const FIBITMAPUniquePtr &bitmap) :
		mSize(FreeImage_GetWidth(bitmap.get()), FreeImage_GetHeight(bitmap.get())),
		mPitch(FreeImage_GetPitch(bitmap.get())),
		mBits(FreeImage_GetBits(bitmap.get()))
	{
		Q_ASSERT(mPitch >= mSize.width() * int(sizeof(TPixel)));
	}

	FIBITMAPWrapper(const FIBITMAPWrapper &) = delete;
	FIBITMAPWrapper(FIBITMAPWrapper &&) = delete;
	FIBITMAPWrapper &operator=(const FIBITMAPWrapper &) = delete;
	FIBITMAPWrapper &operator=(FIBITMAPWrapper &&) = delete;

	iterator scanline(int y)
	{
		Q_ASSERT(0 <= y && y < mSize.height());
		y = mSize.height() - y - 1;
		auto p = mBits + mPitch * y;
		return reinterpret_cast<iterator>(p);
	}

	const_iterator constScanline(int y) const
	{
		Q_ASSERT(0 <= y && y < mSize.height());
		y = mSize.height() - y - 1;
		auto p = mBits + mPitch * y;
		return reinterpret_cast<const_iterator>(p);
	}

	QSize size() const
	{
		return mSize;
	}

private:

	QSize mSize;
	int mPitch;
	uint8_t *mBits;
};

template <class TImage>
static bool pasteFIBITMAPToImage(const QPoint &pos, TImage &dst, const FIBITMAPUniquePtr &bitmap)
{
	auto type = FreeImage_GetImageType(bitmap.get());
	
	switch (type)
	{
		case FIT_BITMAP:
		{
			int bpp = FreeImage_GetBPP(bitmap.get());

			switch (bpp)
			{
				case 32:
				{
					dst.paste(FIBITMAPWrapper<BgraU8>(bitmap), pos);
					break;
				}
				default:
				{
					FIBITMAPUniquePtr newBitmap(FreeImage_ConvertTo32Bits(bitmap.get()));
					dst.paste(FIBITMAPWrapper<BgraU8>(newBitmap), pos);
					break;
				}
			}
			break;
		}
		case FIT_RGB16:
		{
			dst.paste(FIBITMAPWrapper<RgbU16>(bitmap), pos);
			break;
		}
		case FIT_RGBA16:
		{
			dst.paste(FIBITMAPWrapper<RgbaU16>(bitmap), pos);
			break;
		}
		default:
		{
			qWarning() << Q_FUNC_INFO << ": Unsupported data type";
			return false;
		}
	}
	
	return true;
}

template <class TImage>
static bool pasteImageToFIBITMAP(const QPoint &pos, const FIBITMAPUniquePtr &bitmap, const TImage &src)
{
	auto type = FreeImage_GetImageType(bitmap.get());
	
	switch (type)
	{
		case FIT_BITMAP:
		{
			int bpp = FreeImage_GetBPP(bitmap.get());
			
			switch (bpp)
			{
				case 24:
				{
					FIBITMAPWrapper<BgrU8> wrapper(bitmap);
					wrapper.paste(src, pos);
					break;
				}
				case 32:
				{
					FIBITMAPWrapper<BgraU8> wrapper(bitmap);
					wrapper.paste(src, pos);
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
			FIBITMAPWrapper<RgbU16> wrapper(bitmap);
			wrapper.paste(src, pos);
			break;
		}
		case FIT_RGBA16:
		{
			FIBITMAPWrapper<RgbaU16> wrapper(bitmap);
			wrapper.paste(src, pos);
			break;
		}
		default:
			qWarning() << Q_FUNC_INFO << ": Unsupported data type";
			return false;
	}
	
	return true;
}


struct ImageReader::Data
{
	FIBITMAPUniquePtr bitmap;
	QSize size;
};

ImageReader::ImageReader() :
    d(new Data)
{}

ImageReader::~ImageReader()
{
	delete d;
}

bool ImageReader::read(QIODevice *device)
{
	FreeImage_SetOutputMessage(outputMessage);
	
	FreeImageIO io;
	io.read_proc = readFromQIODevice;
	io.write_proc = 0;
	io.seek_proc = seekQIODevice;
	io.tell_proc = tellQIODevice;
	
	auto format = FreeImage_GetFileTypeFromHandle(&io, device);
	
	if (format != FIF_UNKNOWN) {
		int flags = 0;
		if (format == FIF_JPEG)
			flags = JPEG_ACCURATE;
		d->bitmap.reset(FreeImage_LoadFromHandle(format, &io, device, flags));
	}
	
	if (d->bitmap) {
		int w = FreeImage_GetWidth(d->bitmap.get());
		int h = FreeImage_GetHeight(d->bitmap.get());
		d->size = QSize(w, h);
	} else {
		qWarning() << "Malachite::ImageImporter::load failed";
	}
	
	return bool(d->bitmap);
}

bool ImageReader::read(const QString &filepath)
{
	QFile file(filepath);
	
	if (!file.open(QIODevice::ReadOnly))
		return false;
	
	return read(&file);
}

bool ImageReader::isValid() const
{
	return bool(d->bitmap);
}

QSize ImageReader::size() const
{
	return d->size;
}

Image ImageReader::toImage() const
{
	if (!isValid())
		return Image();
	
	Image image(size());
	
	pasteFIBITMAPToImage(QPoint(), image, d->bitmap);
	return image;
}

Surface ImageReader::toSurface(const QPoint &p) const
{
	if (!isValid())
		return Surface();
	
	Surface surface;
	pasteFIBITMAPToImage(p, surface, d->bitmap);
	surface.squeeze();
	return surface;
}

QStringList ImageReader::readableExtensions()
{
	return { "bmp", "png", "jpg", "jpeg" };
}



struct ImageWriter::Data
{
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
	
	void allocate(const QSize &size)
	{
		switch (format) {
			case FIF_PNG:
				if (alphaEnabled)
					bitmap.reset(FreeImage_AllocateT(FIT_RGBA16, size.width(), size.height()));
				else
					bitmap.reset(FreeImage_AllocateT(FIT_RGB16, size.width(), size.height()));
				break;
			default:
				bitmap.reset(FreeImage_Allocate(size.width(), size.height(), 24));
		}
	}
	
	QSize size;
	FIBITMAPUniquePtr bitmap;
	FREE_IMAGE_FORMAT format;
	int quality = 80;
	bool alphaEnabled = true;
};

ImageWriter::ImageWriter(const QString &format) :
    d(new Data)
{
	d->setFormatString(format);
}

ImageWriter::~ImageWriter()
{
	delete d;
}

void ImageWriter::setQuality(int quality)
{
	d->quality = quality;
}

int ImageWriter::quality() const
{
	return d->quality;
}

void ImageWriter::setAlphaEnabled(bool enabled)
{
	d->alphaEnabled = enabled;
}

bool ImageWriter::isAlphaEnabled() const
{
	return d->alphaEnabled;
}

bool ImageWriter::write(QIODevice *device)
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
	
	return FreeImage_SaveToHandle(d->format, d->bitmap.get(), &io, device, flags);
}

bool ImageWriter::write(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	
	return write(&file);
}

bool ImageWriter::setImage(const Image &image)
{
	d->allocate(image.size());
	if (!d->bitmap)
		return false;
	
	return pasteImage(image, QPoint());
}

bool ImageWriter::setSurface(const Surface &surface, const QRect &rect)
{
	auto size = rect.size();
	
	d->allocate(size);
	if (!d->bitmap)
		return false;
	
	auto pos = rect.topLeft();
	
	for (const QPoint &key : Surface::rectToKeys(QRect(pos, size))) {
		if (pasteImage(surface.tile(key), key * Surface::tileWidth() - pos) == false)
			return false;
	}
	
	return true;
}

bool ImageWriter::pasteImage(const Image &image, const QPoint &pos)
{
	if (!d->bitmap)
		return false;
	
	return pasteImageToFIBITMAP(pos, d->bitmap, image);
}


}

