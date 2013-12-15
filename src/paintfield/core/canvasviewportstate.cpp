#include "canvasviewportstate.h"

#include "layerrenderer.h"
#include "canvas.h"
#include "document.h"
#include "selection.h"
#include "layerscene.h"
#include "tool.h"
#include <QImage>
#include <QPainter>

namespace PaintField {

namespace {

class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer(Tool *tool) : mTool(tool) {}

protected:

	void drawLayer(Malachite::SurfacePainter *painter, const LayerConstRef &layer) override
	{
		if (mTool && mTool->layerDelegations().contains(layer))
			mTool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}

	void renderChildren(Malachite::SurfacePainter *painter, const LayerConstRef &parent) override
	{
		if (!mTool || mTool->layerInsertions().isEmpty()) {
			LayerRenderer::renderChildren(painter, parent);
		} else {
			auto originalLayers = parent->children();
			auto layers = originalLayers;

			for (auto insertion : mTool->layerInsertions()) {
				if (insertion.parent == parent) {
					int index = insertion.index;
					auto layer = insertion.layer;
					if (index == originalLayers.size()) {
						layers << layer;
					} else {
						auto layerAt = originalLayers.at(index);
						int trueIndex = layers.indexOf(layerAt);
						layers.insert(trueIndex, layer);
					}
				}
			}
			renderLayers(painter, layers);
		}
	}

private:

	Tool *mTool = 0;
};

template <typename TFunction>
void drawDivided(const QRect &viewRect, const TFunction &drawFunc)
{
	constexpr auto unit = 128;

	if (viewRect.width() * viewRect.height() <= unit * unit)
	{
		drawFunc(viewRect);
	}
	else
	{
		int xCount = viewRect.width() / unit;
		if (viewRect.width() % unit)
			xCount++;

		int yCount = viewRect.height() / unit;
		if (viewRect.height() % unit)
			yCount++;

		for (int x = 0; x < xCount; ++x)
		{
			for (int y = 0; y < yCount; ++y)
			{
				auto viewRectDivided = viewRect & QRect(viewRect.topLeft() + QPoint(x, y) * unit, QSize(unit, unit));
				drawFunc(viewRectDivided);
			}
		}
	}
}

} // anonymous namespace

void CanvasViewportState::setCanvas(Canvas *canvas)
{
	this->mCanvas = canvas;
	this->mSelection = canvas->document()->selection();
}

void CanvasViewportState::render(QPainter *painter, const QRect &windowRepaintRect)
{
	bool retinaMode = this->mRetinaMode;

	auto fromWindowRect = [&](const QRect &rect) -> QRect {
		if (retinaMode)
			return QRect(rect.left() * 2, rect.top() * 2, rect.width() * 2, rect.height() * 2);
		else
			return rect;
	};

	auto toWindowRect = [&](const QRect &rect) -> QRect {
		if (retinaMode)
			return QRect(rect.left() / 2, rect.top() / 2, rect.width() / 2, rect.height() / 2);
		else
			return rect;
	};

	auto repaintRect = fromWindowRect(windowRepaintRect);
	auto sceneRepaintRect = this->mTransforms->viewToScene.mapRect(QRectF(repaintRect)).toAlignedRect();
	auto surface = this->mMipmap.surface();
	auto selectionSurface = this->mSelectionMipmap.surface();
	auto hasSelection = selectionSurface.hasTileInRect(sceneRepaintRect);
	auto selectionRgb = qRgba(0 ,0 ,100, 100);

	painter->setCompositionMode(QPainter::CompositionMode_Source);

	// get rid of unnecessary state change for performance
	if (!(windowRepaintRect & QRect(QPoint(), this->mDocumentSize)).isEmpty()) {
		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor(128, 128, 128));
	}

	auto cropSurface = [&](const QRect &rect)
	{
		//PAINTFIELD_DEBUG << state->cacheRect << rect;
		if (this->mCacheAvailable && this->mCacheRect == rect) {
			//PAINTFIELD_DEBUG << "cached";
			return this->mCacheImage;
		} else {
			return surface.crop(rect);
		}
	};

	if (this->mTranslationOnly) // easy, view is only translated
	{
		//PAINTFIELD_DEBUG << "translation only";

		auto viewRectToSceneRect = [&](const QRect &rect) {
			return rect.translated(this->mTranslationToScene);
		};

		auto drawInViewRect = [&](const QRect &viewRect)
		{
			auto sceneRect = viewRectToSceneRect(viewRect);
			auto windowRect = toWindowRect(viewRect);

			if ((sceneRect & QRect(QPoint(), this->mDocumentSize)).isEmpty()) {
				painter->drawRect(windowRect);
			} else {
				painter->drawImage(windowRect, Malachite::wrapInQImage(cropSurface(sceneRect)));
			}
		};

		auto drawSelectionInViewRect = [&](const QRect &viewRect) {
			auto sceneRect = viewRectToSceneRect(viewRect);
			if (!selectionSurface.hasTileInRect(sceneRect))
				return;
			auto windowRect = toWindowRect(viewRect);
			painter->drawImage(windowRect, selectionSurface.crop(sceneRect).toQImageARGBPremult(selectionRgb));
		};

		drawDivided(repaintRect, drawInViewRect);

		if (hasSelection) {
			painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
			drawDivided(repaintRect, drawSelectionInViewRect);
		}
	}
	else
	{
		//PAINTFIELD_DEBUG << "transformed";
		auto viewRectToSceneRect = [&](const QRect &rect) {
			return this->mTransforms->viewToMipmap.mapRect(QRectF(rect)).toAlignedRect();
		};

		auto drawInViewRect = [&](const QRect &viewRect) {

			auto sceneRect = viewRectToSceneRect(viewRect);
			auto croppedImage = cropSurface(sceneRect);

			QImage image(viewRect.size(), QImage::Format_ARGB32_Premultiplied);
			{
				QPainter imagePainter(&image);
				imagePainter.setCompositionMode(QPainter::CompositionMode_Source);

				if (this->mTransforms->scale < 2.0)
					imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);

				imagePainter.setTransform( this->mTransforms->mipmapToView * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
				imagePainter.drawImage(sceneRect.topLeft(), wrapInQImage(croppedImage));
			}

			if ((sceneRect & QRect(QPoint(), this->mDocumentSize)).isEmpty())
				painter->drawRect(toWindowRect(viewRect));
			else
				painter->drawImage(toWindowRect(viewRect), image);
		};

		auto drawSelectionInViewRect = [&](const QRect &viewRect) {

			auto sceneRect = viewRectToSceneRect(viewRect);
			if (!selectionSurface.hasTileInRect(sceneRect))
				return;
			auto croppedImage = selectionSurface.crop(sceneRect);

			SelectionImage image(viewRect.size());
			{
				QPainter imagePainter(&image.qimage());
				imagePainter.setCompositionMode(QPainter::CompositionMode_Source);
				imagePainter.setTransform( this->mTransforms->mipmapToView * QTransform::fromTranslate(-viewRect.left(), -viewRect.top()) );
				imagePainter.drawImage(sceneRect.topLeft(), croppedImage.qimage());
			}

			painter->drawImage(toWindowRect(viewRect), image.toQImageARGBPremult(selectionRgb));
		};

		drawDivided(repaintRect, drawInViewRect);

		if (hasSelection) {
			painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
			drawDivided(repaintRect, drawSelectionInViewRect);
		}
	}
}

void CanvasViewportState::setTransforms(const SP<const CanvasTransforms> &transforms)
{
	this->mMipmap.setCurrentLevel(transforms->mipmapLevel);
	this->mSelectionMipmap.setCurrentLevel(transforms->mipmapLevel);
	this->mTransforms = transforms;
	this->mTranslationOnly = (transforms->mipmapScale == 1.0 && transforms->rotation == 0.0 && !transforms->mirrored);
	this->mTranslationToScene = QPointF(transforms->viewToMipmap.dx(), transforms->viewToMipmap.dy()).toPoint();
}

void CanvasViewportState::setDocumentSize(const QSize &size)
{
	this->mDocumentSize = size;
	this->mMipmap.setSceneSize(size);
	this->mSelectionMipmap.setSceneSize(size);
}

QRect CanvasViewportState::updateTiles(const boost::variant<QPointSet, QHash<QPoint, QRect>> &keysOrRectForKeys)
{
	int rectCount;

	QPointSet keys;
	QHash<QPoint, QRect> rectsForKeys;

	if (keysOrRectForKeys.which() == 0) {
		keys = boost::get<QPointSet>(keysOrRectForKeys);
		rectCount = keys.size();
	} else {
		rectsForKeys = boost::get<QHash<QPoint, QRect>>(keysOrRectForKeys);
		rectCount = rectsForKeys.size();
	}

	QRect rectToBeRepainted;
	QVector<QRect> rects;
	rects.reserve(rectCount);

	// render layers
	Malachite::Surface surface;
	{
		CanvasRenderer renderer(this->mTool);
		surface = renderer.renderToSurface({mCanvas->document()->layerScene()->rootLayer()}, keys, rectsForKeys);
	}

	auto documentRect = QRect(QPoint(), this->mDocumentSize);

	const Malachite::Pixel whitePixel(1.f);
	auto blendOp = Malachite::BlendMode(Malachite::BlendMode::DestinationOver).op();

	auto updateTile = [&](const QPoint &key, const QRect &unclippedRelativeRect) {

		auto relativeDocumentRect = documentRect.translated(-key * Malachite::Surface::tileWidth());
		auto relativeRect = unclippedRelativeRect & relativeDocumentRect;
		if (relativeRect.isEmpty())
			return;

		auto absoluteRect = relativeRect.translated(key * Malachite::Surface::tileWidth());

		auto image = surface.crop(absoluteRect);
		blendOp->blend(image.area(), image.begin(), whitePixel);

		auto imageU8 = image.toImageU8();
		this->mMipmap.replace(imageU8, key, relativeRect.topLeft());

		rectToBeRepainted |= absoluteRect;
		rects << absoluteRect;

		if (rectCount == 1) {
			this->mCacheAvailable = true;
			this->mCacheRect = absoluteRect;
			this->mCacheImage = imageU8;
		} else {
			this->mCacheAvailable = false;
		}
	};

	if (keysOrRectForKeys.which() == 1) {
		for (auto iter = rectsForKeys.begin(); iter != rectsForKeys.end(); ++iter)
			updateTile(iter.key(), iter.value());
	} else {
		for (const QPoint &key : keys) {
			const auto rect = QRect(QPoint(), Malachite::Surface::tileSize());
			updateTile(key, rect);
		}
	}

	return this->mTransforms->sceneToWindow.mapRect(rectToBeRepainted);
}

QRect CanvasViewportState::updateSelectionTiles(const QPointSet &keys)
{
	this->mSelectionMipmap.replace(this->mSelection->surface(), keys);
	auto sceneRect = keys++.foldLeft(QRect(), [](const QRect &memo, const QPoint &key) {
		return memo | SelectionSurface::keyToRect(key);
	});
	return this->mTransforms->sceneToWindow.mapRect(sceneRect);
}

} // namespace PaintField

