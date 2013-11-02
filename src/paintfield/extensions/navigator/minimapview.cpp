#include <QPainter>

#include "paintfield/core/canvas.h"
#include "paintfield/core/canvasview.h"
#include "paintfield/core/layerscene.h"

#include "minimapview.h"

namespace PaintField {

struct MinimapView::Data
{
	Canvas *canvas = nullptr;
	QPixmap thumbnailPixmap;
	QPointSet keys;
};

MinimapView::MinimapView(Canvas *canvas, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	if (canvas)
	{
		d->canvas = canvas;
		connect(canvas->document()->layerScene(), SIGNAL(thumbnailsUpdated(QPointSet)), this, SLOT(onThumbnailUpdated(QPointSet)));
		connect(canvas, SIGNAL(transformsChanged(SP<const CanvasTransforms>)), this, SLOT(onTransformChanged()));
		d->thumbnailPixmap = QPixmap(canvas->document()->size());
		d->thumbnailPixmap.fill(Qt::white);
		d->keys = canvas->document()->tileKeys();
	}

	setMinimumHeight(100);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

MinimapView::~MinimapView()
{
	delete d;
}

void MinimapView::onThumbnailUpdated(const QPointSet &keys)
{
	PAINTFIELD_DEBUG << keys;

	auto surface = d->canvas->viewController()->mergedSurface();

	{
		QPainter painter(&d->thumbnailPixmap);
		painter.setCompositionMode(QPainter::CompositionMode_Source);

		for (const auto &key : keys)
			painter.drawImage(key * surface.tileWidth(), surface.tile(key).wrapInQImage());
	}

	update();
}

void MinimapView::onTransformChanged()
{

}

void MinimapView::resizeEvent(QResizeEvent *)
{

}

void MinimapView::paintEvent(QPaintEvent *)
{
	if (d->thumbnailPixmap.isNull())
		return;

	QRectF pixmapRect;
	{
		auto viewWidth = double(this->width());
		auto viewheight = double(this->height());
		auto imageWidth = double(d->thumbnailPixmap.width());
		auto imageHeight = double(d->thumbnailPixmap.height());
		auto vratio = viewWidth / imageWidth;
		auto hratio = viewheight / imageHeight;
		auto ratio = std::min(vratio, hratio);

		auto vcenter = viewheight * 0.5;
		auto hcenter = viewWidth * 0.5;
		auto resultWidth = imageWidth * ratio;
		auto resultHeight = imageHeight * ratio;

		pixmapRect = QRectF(hcenter - resultWidth * 0.5, vcenter - resultHeight * 0.5, resultWidth, resultHeight);
	}
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.drawPixmap(pixmapRect, d->thumbnailPixmap, d->thumbnailPixmap.rect());
}

} // namespace PaintField
