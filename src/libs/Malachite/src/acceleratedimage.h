#ifndef MLACCELERATEDIMAGE_H
#define MLACCELERATEDIMAGE_H

#include "image.h"
#include "gl3.h"

class MLAcceleratedImage
{
public:
	MLAcceleratedImage(const QSize &size);
	MLAcceleratedImage(const MLImage &image);
	~MLAcceleratedImage();
	
	QSize size() const { return _size; }
	MLImage toImage() const;
	
	GLuint textureId() { return _texId; }
	
private:
	
	void loadTexture(const void *data);
	
	QSize _size;
	GLuint _texId;
};

#endif // MLACCELERATEDIMAGE_H
