#include "mlacceleratedimage.h"

MLAcceleratedImage::MLAcceleratedImage(const QSize &size) :
	_size(size),
	_texId(0)
{
	loadTexture(0);
}

MLAcceleratedImage::MLAcceleratedImage(const MLImage &image) :
	_size(image.size()),
	_texId(0)
{
	loadTexture(image.constScanline(0));
}

MLAcceleratedImage::~MLAcceleratedImage()
{
	glDeleteTextures(1, &_texId);
}

MLImage MLAcceleratedImage::toImage() const
{
	MLImage image(_size);
	glBindTexture(GL_TEXTURE_2D, _texId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_FLOAT, image.scanline(0));
	glBindTexture(GL_TEXTURE_2D, 0);
	return image;
}

void MLAcceleratedImage::loadTexture(const void *data)
{
	glGenTextures(1, &_texId);
	glBindTexture(GL_TEXTURE_2D, _texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _size.width(), _size.height(), 0, GL_BGRA, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
