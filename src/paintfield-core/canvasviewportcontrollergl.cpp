#include <Malachite/Surface>
#include <Malachite/Vec2D>
#include "canvasviewportcontrollergl.h"

using namespace Malachite;

namespace PaintField {

struct vec2
{
	vec2() {}
	vec2(float x, float y) : x(x), y(y) {}
	vec2(const vec2 &other) = default;
	float x, y;
};

struct CanvasViewportGL::Data
{
	GLuint texture = 0;
	vec2 vertices[4];
	vec2 texCoords[4];
	QSize size;
};

CanvasViewportGL::CanvasViewportGL(QWidget *parent) :
    QGLWidget(parent),
    d(new Data)
{
	
}

CanvasViewportGL::~CanvasViewportGL()
{
	delete d;
}

static int powerOf2Bound(int x)
{
	int p = 1;
	
	for (int i = 0; true; ++i)
	{
		if (p >= x)
			return p;
		p *= 2;
	}
}

void CanvasViewportGL::setDocumentSize(const QSize &size)
{
	int texWidth = powerOf2Bound(size.width());
	int texHeight = powerOf2Bound(size.height());
	
	float s = float(texWidth) / float(size.width());
	float t = float(texHeight) / float(size.height());
	d->texCoords[0] = vec2(0, 0);
	d->texCoords[1] = vec2(s, 0);
	d->texCoords[2] = vec2(s, t);
	d->texCoords[3] = vec2(0, t);
	
	glGenTextures ( 1, &d->texture );
	glBindTexture ( GL_TEXTURE_2D, d->texture );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D ( GL_TEXTURE_2D, 0, 3, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
}

void CanvasViewportGL::setTransform(const QTransform &transform)
{
	
}

void CanvasViewportGL::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	if (d->texture)
	{
		QPoint totalOffset = tileKey * Surface::TileSize + offset;
		
		glBindTexture(GL_TEXTURE_2D, d->texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, totalOffset.x(), totalOffset.y(), image.width(), image.height(), GL_BGRA, GL_FLOAT, image.constBits());
	}
}

void CanvasViewportGL::initializeGL()
{
	glEnable ( GL_TEXTURE_2D );
	
	emit ready();
}

void CanvasViewportGL::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0, w, h, 0, 0, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CanvasViewportGL::paintGL()
{
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindTexture(GL_TEXTURE_2D, d->texture);
	
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(0.5f, 0.5f);
	glTexCoord2f(1, 0);
	glVertex2f(0.5f, -0.5f);
	glTexCoord2f(0, 0);
	glVertex2f(-0.5f, -0.5f);
	glTexCoord2f(0, 1);
	glVertex2f(-0.5f, 0.5f);
	glEnd();
}

struct CanvasViewportControllerGL::Data
{
	CanvasViewportGL *view;
};

CanvasViewportControllerGL::CanvasViewportControllerGL(QObject *parent) :
    AbstractCanvasViewportController(parent),
    d(new Data)
{
	d->view = new CanvasViewportGL();
	connect(d->view, SIGNAL(ready()), this, SIGNAL(ready()));
}

CanvasViewportControllerGL::~CanvasViewportControllerGL()
{
	delete d;
}

QWidget *CanvasViewportControllerGL::view()
{
	return d->view;
}

void CanvasViewportControllerGL::setDocumentSize(const QSize &size)
{
	d->view->setDocumentSize(size);
}

void CanvasViewportControllerGL::setTransform(const QTransform &transform)
{
	d->view->setTransform(transform);
}

void CanvasViewportControllerGL::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	d->view->updateTile(tileKey, image, offset);
}

void CanvasViewportControllerGL::update()
{
	d->view->updateGL();
}

} // namespace PaintField
