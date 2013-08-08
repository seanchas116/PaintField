#pragma once

//ExportName: ImageIO

#include <QDebug>
#include "surface.h"

struct FIBITMAP;

namespace Malachite
{

class MALACHITESHARED_EXPORT ImageReader
{
public:
	
	ImageReader();
	~ImageReader();
	
	/**
	 * Loads from a QIODevice.
	 * The device must be random-access.
	 * @param device
	 * @return 
	 */
	bool read(QIODevice *device);
	
	bool read(const QString &filepath);
	
	bool isValid() const;
	
	QSize size() const;
	
	Image toImage() const;
	Surface toSurface(const QPoint &p = QPoint()) const;
	
	static QStringList readableExtensions();
	
private:
	
	struct Data;
	Data *d;
};

class MALACHITESHARED_EXPORT ImageWriter
{
public:
	
	ImageWriter(const QString &format);
	~ImageWriter();
	
	/**
	 * Sets the image quality for JPEG files.
	 * @param quality 0..100
	 */
	void setQuality(int quality);
	
	int quality() const;
	
	/**
	 * Sets the alpha channe is enabled when the image is written.
	 * This function must be called before setImage or setSurface is called.
	 * @param enabled
	 */
	void setAlphaEnabled(bool enabled);
	
	bool isAlphaEnabled() const;
	
	/**
	 * Sets the image.
	 * @param image
	 * @return 
	 */
	bool setImage(const Image &image);
	
	/**
	 * Sets the specified region in surface.
	 * @param surface
	 * @param rect
	 * @return 
	 */
	bool setSurface(const Surface &surface, const QRect &rect);
	
	bool setSurface(const Surface &surface, const QSize &size) { return setSurface(surface, QRect(QPoint(), size)); }
	
	/**
	 * Writes the saved image into device.
	 * @param device
	 * @return 
	 */
	bool write(QIODevice *device);
	bool write(const QString &filePath);
	
private:
	
	bool pasteImage(const Image &image, const QPoint &pos);
	
	struct Data;
	Data *d;
};

}
