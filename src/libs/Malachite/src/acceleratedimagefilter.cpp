#include <QtCore>
#include "glu.h"
#include "mlacceleratedimagefilter.h"

#define LOG_BUFFER_SIZE	1024

MLAcceleratedImageFilter::MLAcceleratedImageFilter(GLuint texId, const QSize &size) :
	_size(size),
	_texId(texId)
{
	init();
}

MLAcceleratedImageFilter::MLAcceleratedImageFilter(MLAcceleratedImage *image) :
	_size(image->size()),
	_texId(image->textureId())
{
	init();
}

MLAcceleratedImageFilter::~MLAcceleratedImageFilter()
{
	glDeleteFramebuffers(1, &_fboId);
	glDeleteRenderbuffers(1, &_rboId);
}

void MLAcceleratedImageFilter::init()
{
	glGenRenderbuffers(1, &_rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, _rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.width(), _size.height());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	glGenFramebuffers(1, &_fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboId);
	GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (stat != GL_FRAMEBUFFER_COMPLETE)
		qWarning() << Q_FUNC_INFO << ": could not create framebuffer";
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MLAcceleratedImageFilter::addInputImage(GLuint id, const QByteArray &name)
{
	if (_inTexInfos.size() >= MaxInImageCount)
	{
		qWarning() << Q_FUNC_INFO << ": too many input images";
		return;
	}
	
	_inTexInfos << TextureInfo(id, name);
}

void MLAcceleratedImageFilter::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	_program.bind();
	
	bindInTextures();
	
	for (int i = 0; i < _inTexInfos.size(); ++i)
	{
		GLuint loc = glGetUniformLocation(_program.programId(), _inTexInfos.at(i).name.constData());
		glUniform1i(loc, i);
		//_program.setUniformValue(_inTexInfos.at(i).name.data(), i);
	}
	
	//GLuint viewportLoc = glGetUniformLocation(_program.programId(), "ml_CoordFactor");
	//glUniform2f(viewportLoc, 1.0 / _size.width(), 1.0 / _size.height());
	//_program.setUniformValue("ml_Viewport", _size.width(), _size.height());
	
	glViewport(0, 0, _size.width(), _size.height());
	
	//glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glVertex2i(-1, -1);
	glVertex2i(1, -1);
	glVertex2i(1, 1);
	glVertex2i(-1, 1);
	glEnd();
	
	releaseInTextures();
	
	_program.release();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MLAcceleratedImageFilter::bindInTextures()
{
	int count = _inTexInfos.size();
	if (!count) return;
	glActiveTexture(GL_TEXTURE0);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(0).id);
	glActiveTexture(GL_TEXTURE1);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(1).id);
	glActiveTexture(GL_TEXTURE2);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(2).id);
	glActiveTexture(GL_TEXTURE3);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(3).id);
	glActiveTexture(GL_TEXTURE4);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(4).id);
	glActiveTexture(GL_TEXTURE5);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(5).id);
	glActiveTexture(GL_TEXTURE6);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(6).id);
	glActiveTexture(GL_TEXTURE7);
	if(count--) glBindTexture(GL_TEXTURE_2D, _inTexInfos.at(7).id);
	glActiveTexture(GL_TEXTURE0);
}

void MLAcceleratedImageFilter::releaseInTextures()
{
	int count = _inTexInfos.size();
	if (!count) return;
	glActiveTexture(GL_TEXTURE0);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE6);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE7);
	if(count--) glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}
