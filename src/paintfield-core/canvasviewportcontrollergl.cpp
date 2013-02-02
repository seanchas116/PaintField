#include <Malachite/Surface>
#include <Malachite/Vec2D>
#include <QDebug>
#include "drawutil.h"

#include "canvasviewportcontrollergl.h"

using namespace Malachite;

namespace PaintField {

struct CanvasViewportGL::Data
{
	GLuint texture = 0, pbo = 0;
	
	QSize sceneSize;
	Vec2D textureSizeInv;
	Affine2D transformNormalize, transformToNormalizedView, transformToView, transformToScene;
	
	std::array<Vec2D, 4> texCoords;
	std::array<Vec2D, 4> vertices;
	
	Vec2D texturePos(const Vec2D &scenePos)
	{
		return scenePos * textureSizeInv;
	}
	
	QRectF mapAlignedRectToView(const QRectF &sceneRect)
	{
		auto topLeft = transformToView * Vec2D(sceneRect.topLeft());
		auto topRight = transformToView * Vec2D(sceneRect.topRight());
		auto bottomLeft = transformToView * Vec2D(sceneRect.bottomLeft());
		auto bottomRight = transformToView * Vec2D(sceneRect.bottomRight());
		
		auto left = qMax(qMax(topLeft.x(), topRight.x()), qMax(bottomLeft.x(), bottomRight.x()));
		auto right = qMin(qMin(topLeft.x(), topRight.x()), qMin(bottomLeft.x(), bottomRight.x()));
		auto top = qMax(qMax(topLeft.y(), topRight.y()), qMax(bottomLeft.y(), bottomRight.y()));
		auto bottom = qMin(qMax(topLeft.y(), topRight.y()), qMin(bottomLeft.y(), bottomRight.y()));
		
		left = std::floor(left);
		right = std::ceil(right);
		top = std::floor(top);
		bottom = std::ceil(bottom);
		
		return QRectF(left, top, right - left, bottom - top);
	}
	
	Vec2D normalizedPos(const Vec2D &viewPos)
	{
		return transformNormalize * viewPos;
	}
	
	void texCoord(const Vec2D &pos)
	{
		glTexCoord2d(pos.x(), pos.y());
	}
	
	void vertex(const Vec2D &pos)
	{
		glVertex2d(pos.x(), pos.y());
	}
	
	void quadTo(const Vec2D &viewPos)
	{
		texCoord(texturePos(transformToScene * viewPos));
		vertex(transformNormalize * viewPos);
	}
	
	void repaintRect(const QRectF &sceneRect)
	{
		auto viewRect = mapAlignedRectToView(sceneRect);
		PAINTFIELD_DEBUG << viewRect;
		
		glBegin(GL_QUADS);
		
		quadTo(viewRect.topLeft());
		quadTo(viewRect.topRight());
		quadTo(viewRect.bottomRight());
		quadTo(viewRect.bottomLeft());
		
		glEnd();
	}
	
	void drawWhole()
	{
		glBegin(GL_QUADS);
		
		for (int i = 0; i < 4; ++i)
		{
			texCoord(texCoords[i]);
			vertex(vertices[i]);
		}
		
		glEnd();
	}
};

CanvasViewportGL::CanvasViewportGL(const QGLFormat &format, QWidget *parent) :
    QGLWidget(format, parent),
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
	//int texWidth = 128;
	//int texHeight = 128;
	
	double s = double(size.width()) / double(texWidth);
	double t = double(size.height()) / double(texHeight);
	
	d->texCoords[0] = Vec2D(0, 0);
	d->texCoords[1] = Vec2D(s, 0);
	d->texCoords[2] = Vec2D(s, t);
	d->texCoords[3] = Vec2D(0, t);
	
	glGenTextures ( 1, &d->texture );
	glBindTexture ( GL_TEXTURE_2D, d->texture );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D ( GL_TEXTURE_2D, 0, 3, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
	
	d->sceneSize = size;
	d->textureSizeInv = Vec2D(1) / Vec2D(texWidth, texHeight);
}

void CanvasViewportGL::setTransform(const Affine2D &transform)
{
	double w = width();
	double h = height();
	
	PAINTFIELD_DEBUG << transform.toQTransform();
	
	Affine2D tNormalize(2.0 / w, 0, 0, -2.0 / h, -1.0, 1.0);
	
	auto t = tNormalize * transform;
	
	PAINTFIELD_DEBUG << t.toQTransform();
	
	PAINTFIELD_DEBUG << d->sceneSize;
	
	d->vertices[0] = t * Vec2D(0, 0);
	d->vertices[1] = t * Vec2D(d->sceneSize.width(), 0);
	d->vertices[2] = t * Vec2D(d->sceneSize.width(), d->sceneSize.height());
	d->vertices[3] = t * Vec2D(0, d->sceneSize.height());
	
	d->transformToView = transform;
	d->transformToScene = transform.inverted();
	d->transformToNormalizedView = t;
	d->transformNormalize = tNormalize;
}

void CanvasViewportGL::updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset)
{
	if (d->texture)
	{
		QPoint totalOffset = tileKey * Surface::TileSize + offset;
		
		QSize size = image.size();
		int pixelCount = size.width() * size.height();
		
		QScopedArrayPointer<BgraPremultU8> data(new BgraPremultU8[pixelCount]);
		DrawUtil::copyColorFast(pixelCount, data.data(), image.constBits());
		
		auto buf = (uint8_t *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (buf)
		{
			int count = size.width() * 4;
			
			auto p = data.data();
			p += count * (size.height() - 1);
			
			memcpy(buf, p, count);
			buf += count;
			p -= count;
			
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		}
		
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, );
		
		QPoint rasterOffset = totalOffset + d->transformToView.delta().toQPoint();
		rasterOffset.ry() = height() - rasterOffset.y() - Surface::TileSize;
		
		glWindowPos2i(rasterOffset.x(), rasterOffset.y());
		glDrawPixels(size.width(), size.height(), GL_BGRA, GL_UNSIGNED_BYTE, 0);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, totalOffset.x(), totalOffset.y(), size.width(), size.height(), GL_BGRA, GL_UNSIGNED_BYTE, 0);
		//d->repaintRect(QRect(totalOffset, size));
	}
}

void CanvasViewportGL::beforeUpdateTile()
{
	makeCurrent();
	glBindTexture(GL_TEXTURE_2D, d->texture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->pbo);
}

void CanvasViewportGL::afterUpdateTile()
{
	//swapBuffers();
	glFlush();
}

void CanvasViewportGL::initializeGL()
{
	glEnable ( GL_TEXTURE_2D );
	glEnable ( GL_PIXEL_UNPACK_BUFFER );
	
	glGenBuffers(1, &d->pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, Surface::TileSize * Surface::TileSize * 4, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	
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
	
	d->drawWhole();
}

struct CanvasViewportControllerGL::Data
{
	CanvasViewportGL *view;
};

CanvasViewportControllerGL::CanvasViewportControllerGL(QObject *parent) :
    AbstractCanvasViewportController(parent),
    d(new Data)
{
	
	QGLFormat format;
	format.setDepth(false);
	format.setDoubleBuffer(false);
	format.setStencil(false);
	
	d->view = new CanvasViewportGL(format);
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

void CanvasViewportControllerGL::beforeUpdateTile()
{
	d->view->beforeUpdateTile();
}

void CanvasViewportControllerGL::afterUpdateTile()
{
	d->view->afterUpdateTile();
}

void CanvasViewportControllerGL::update()
{
	d->view->updateGL();
}

} // namespace PaintField
