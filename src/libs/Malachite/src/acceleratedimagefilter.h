#ifndef MLACCELERATEDIMAGEFILTER_H
#define MLACCELERATEDIMAGEFILTER_H

#include <QtOpenGL>
#include "mlacceleratedimage.h"

class MLAcceleratedImageFilter
{
public:
	
	enum
	{
		MaxInImageCount = 8
	};
	
	MLAcceleratedImageFilter(GLuint texId, const QSize &size);
	MLAcceleratedImageFilter(MLAcceleratedImage *image);
	~MLAcceleratedImageFilter();
	
	void addInputImage(GLuint textureId, const QByteArray &name);
	void addInputImage(MLAcceleratedImage *image, const QByteArray &name) { addInputImage(image->textureId(), name); }
	
	void render();
	
	QGLShaderProgram *program() { return &_program; }
	
	QSize size() const { return _size; }
	GLuint textureId() { return _texId; }
	GLuint framebufferId() { return _fboId; }
	
private:
	
	struct TextureInfo
	{
		TextureInfo(GLuint id, const QByteArray &name) : id(id), name(name) {}
		TextureInfo() : id(0) {}
		
		GLuint id;
		QByteArray name;
	};
	
	void init();
	void bindInTextures();
	void releaseInTextures();
	
	QGLShaderProgram _program;
	QSize _size;
	GLuint _texId, _fboId, _rboId;
	
	QVector<TextureInfo> _inTexInfos;
};

#endif // MLACCELERATEDIMAGEFILTER_H
