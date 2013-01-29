#include <Malachite/Surface>
#include <Malachite/Vec2D>
#include <QDebug>
#include "debug.h"
#include "canvasviewportcontrollergl.h"

using namespace Malachite;

namespace PaintField {

struct vec2
{
	vec2() {}
	vec2(float x, float y) : x(x), y(y) {}
	vec2(const vec2 &other) = default;
	vec2(const Vec2D &p) : x(p.x()), y(p.y()) {}
	float x, y;
};

struct CanvasViewportGL::Data
{
	GLuint texture = 0;
	vec2 vertices[4];
	vec2 texCoords[4];
	QSize sceneSize;
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
	
	float s = float(size.width()) / float(texWidth);
	float t = float(size.height()) / float(texHeight);
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
	
	d->sceneSize = size;
}

void CanvasViewportGL::setTransform(const Affine2D &transform)
{
	double w = width();
	double h = height();
	
	PAINTFIELD_DEBUG << transform.toQTransform();
	
	auto t = Affine2D(2.0 / w, 0, 0, -2.0 / h, -1.0, 1.0) * transform;
	
	PAINTFIELD_DEBUG << t.toQTransform();
	
	PAINTFIELD_DEBUG << d->sceneSize;
	
	double sceneWidth = d->sceneSize.width();
	double sceneHeight = d->sceneSize.height();
	
	d->vertices[0] = t * Vec2D(0, 0);
	d->vertices[1] = t * Vec2D(sceneWidth, 0);
	d->vertices[2] = t * Vec2D(sceneWidth, sceneHeight);
	d->vertices[3] = t * Vec2D(0, sceneHeight);
	
	qDebug() << d->vertices[0].x << d->vertices[0].y;
	qDebug() << d->vertices[2].x << d->vertices[2].y;
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
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CanvasViewportGL::paintGL()
{
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindTexture(GL_TEXTURE_2D, d->texture);
	
	glBegin(GL_QUADS);
	
	glTexCoord2f(d->texCoords[0].x, d->texCoords[0].y);
	glVertex2f(d->vertices[0].x, d->vertices[0].y);
	glTexCoord2f(d->texCoords[1].x, d->texCoords[1].y);
	glVertex2f(d->vertices[1].x, d->vertices[1].y);
	glTexCoord2f(d->texCoords[2].x, d->texCoords[2].y);
	glVertex2f(d->vertices[2].x, d->vertices[2].y);
	glTexCoord2f(d->texCoords[3].x, d->texCoords[3].y);
	glVertex2f(d->vertices[3].x, d->vertices[3].y);
	
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

void CanvasViewportControllerGL::setTransform(const Affine2D &transform)
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
