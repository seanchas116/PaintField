#include "canvasviewport.h"

#include "appcontroller.h"
#include "canvasviewportstate.h"
#include "canvastooleventfilter.h"
#include "canvasnavigatoreventfilter.h"
#include "keytracker.h"
#include "tool.h"
#include "canvas.h"
#include "workspace.h"
#include "layerrenderer.h"
#include "document.h"
#include "layerscene.h"
#include "cursorstack.h"
#include "widgets/vanishingscrollbar.h"

#include <QTimer>
#include <QPaintEvent>
#include <QPainter>

namespace PaintField {

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

struct CanvasViewport::Data
{
	CanvasViewport *mSelf = 0;

	Canvas *mCanvas = 0;

	bool mUpdateEnabled = true;
	bool mStroking = false;
	bool mStrokingOrToolEditing = false;

	QPointer<Tool> mTool;
	QCursor mToolCursor;

	CanvasViewportState mState;

	VanishingScrollBar *mScrollBarX = 0, *mScrollBarY = 0;

	void setTransforms(const SP<const CanvasTransforms> &transforms)
	{
		mState.mipmap.setCurrentLevel(transforms->mipmapLevel);
		mState.transforms = transforms;
		mState.translationOnly = (transforms->mipmapScale == 1.0 && transforms->rotation == 0.0 && !transforms->mirrored);
		mState.translationToScene = QPointF(transforms->viewToMipmap.dx(), transforms->viewToMipmap.dy()).toPoint();

		auto maxAbsTranslation = mCanvas->maxAbsoluteTranslation();
		// update scroll bar range
		{
			mScrollBarX->setRange(0, 2 * maxAbsTranslation.x());
			mScrollBarY->setRange(0, 2 * maxAbsTranslation.y());

			mScrollBarX->setPageStep(mSelf->viewSize().width());
			mScrollBarY->setPageStep(mSelf->viewSize().height());
		}

		// update scroll bar value
		{
			mScrollBarX->setValue(maxAbsTranslation.x() - transforms->translation.x());
			mScrollBarY->setValue(maxAbsTranslation.y() - transforms->translation.y());
		}

		mSelf->update();
	}

	void setRetinaMode(bool mode)
	{
		emit mSelf->viewSizeChanged(mSelf->viewSize());
		mState.retinaMode = mode;
		mSelf->update();
	}

	void setDocumentSize(const QSize &size)
	{
		mState.documentSize = size;
		mState.mipmap.setSceneSize(size);
	}

	void setTool(Tool *tool)
	{
		mTool = tool;

		if (tool) {
			auto requestUpdateSet = static_cast<void(Tool::*)(const QPointSet &)>(&Tool::requestUpdate);
			auto requestUpdateHash = static_cast<void(Tool::*)(const QHash<QPoint,QRect> &)>(&Tool::requestUpdate);

			using namespace std::placeholders;
			connect(tool, requestUpdateSet, mSelf, std::bind(&Data::updateTilesWithSet, this, _1));
			connect(tool, requestUpdateHash, mSelf, std::bind(&Data::updateTilesWithHash, this, _1));
			connect(tool, &Tool::editingChanged, mSelf, std::bind(&Data::onStrokingOrToolEditingChanged, this));
			onStrokingOrToolEditingChanged();

			mToolCursor = tool->cursor();
		}
	}

	void updateTilesWithSet(const QPointSet &keys) { updateTiles(keys, QHash<QPoint, QRect>()); }
	void updateTilesWithHash(const QHash<QPoint, QRect> &rects) { updateTiles(QPointSet(), rects); }

	void onClicked()
	{
		mSelf->setFocus();
		mCanvas->workspace()->setCurrentCanvas(mCanvas);
	}

	void onStrokingOrToolEditingChanged()
	{
		if (!mTool)
			return;

		bool strokingOrToolEditing = mStroking || mTool->isEditing();

		if (mStrokingOrToolEditing != strokingOrToolEditing) {
			if (strokingOrToolEditing) {
				mCanvas->document()->layerScene()->abortThumbnailUpdate();
				mCanvas->disableUndoRedo();
			} else {
				mCanvas->enableUndoRedo();
			}
			mStrokingOrToolEditing = strokingOrToolEditing;
		}
	}

	void moveWidgets()
	{
		auto widgetRect = mSelf->geometry();

		int barWidthX = mScrollBarX->totalBarWidth();
		int barWidthY = mScrollBarY->totalBarWidth();

		auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthX, -barWidthY, 0);
		auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthY, 0, 0, -barWidthX);

		mScrollBarX->setGeometry(scrollBarXRect);
		mScrollBarY->setGeometry(scrollBarYRect);
	}

	/// set focus on the viewport and make canvas current
	void activate()
	{
		mSelf->setFocus();
		mCanvas->workspace()->setCurrentCanvas(mCanvas);
	}

	void updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rectForKeys)
	{
		if (!mUpdateEnabled)
			return;

		int keyCount = keys.size();
		int rectForKeysCount = rectForKeys.size();
		int rectCount = rectForKeysCount ? rectForKeysCount : keyCount;

		QRect rectToBeRepainted;
		QVector<QRect> rects;
		rects.reserve(rectCount);

		// render layers
		Malachite::Surface surface;
		{
			CanvasRenderer renderer(mTool);
			surface = renderer.renderToSurface({mCanvas->document()->layerScene()->rootLayer()}, keys, rectForKeys);
		}

		auto documentRect = QRect(QPoint(), mState.documentSize);

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
			mState.mipmap.replace(imageU8, key, relativeRect);

			rectToBeRepainted |= absoluteRect;
			rects << absoluteRect;

			if (rectCount == 1) {
				mState.cacheAvailable = true;
				mState.cacheRect = absoluteRect;
				mState.cacheImage = imageU8;
			} else {
				mState.cacheAvailable = false;
			}
		};

		if (rectForKeysCount) {
			for (auto iter = rectForKeys.begin(); iter != rectForKeys.end(); ++iter)
				updateTile(iter.key(), iter.value());
		} else {
			for (const QPoint &key : keys) {
				const auto rect = QRect(QPoint(), Malachite::Surface::tileSize());
				updateTile(key, rect);
			}
		}

		auto viewRect = mState.transforms->sceneToView.mapRect(rectToBeRepainted);

		if (mState.retinaMode)
			viewRect = QRectF(viewRect.left() * 0.5, viewRect.top() * 0.5, viewRect.width() * 0.5, viewRect.height() * 0.5).toAlignedRect();

		mSelf->repaint(viewRect);
	}
};

CanvasViewportSurface CanvasViewport::mergedSurface() const
{
	return d->mState.mipmap.baseSurface();
}

QSize CanvasViewport::viewSize() const
{
	auto size = this->size();
	if (d->mState.retinaMode)
		size = QSize(size.width() * 2, size.height() * 2);
	return size;
}

void CanvasViewport::setCanvasUpdatesEnabled(bool x)
{
	d->mUpdateEnabled = x;
}

bool CanvasViewport::isCanvasUpdatesEnabled() const
{
	return d->mUpdateEnabled;
}

void CanvasViewport::resizeEvent(QResizeEvent *)
{
	d->moveWidgets();
	emit viewSizeChanged(viewSize());
}

static const QString toolCursorId = "paintfield.canvas.tool";

void CanvasViewport::enterEvent(QEvent *)
{
	d->activate();
	appController()->cursorStack()->add(toolCursorId, d->mToolCursor);
}

void CanvasViewport::leaveEvent(QEvent *)
{
	appController()->cursorStack()->remove(toolCursorId);
}

void CanvasViewport::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);

	QRect rect = event->rect();

	// get rid of unnecessary state change for performance
	if (!(rect & QRect(QPoint(), d->mState.documentSize)).isEmpty()) {
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(128, 128, 128));
	}

	auto draw = [&](const QRect &rect, const QImage &image) {
		painter.drawImage(rect, image);
	};

	auto drawBackground = [&](const QRect &rect) {
		painter.drawRect(rect);
	};

	drawViewport(rect, &d->mState, draw, drawBackground);
}

bool CanvasViewport::event(QEvent *event)
{
	switch (event->type()) {
		case QEvent::TabletPress:
		case QEvent::MouseButtonPress:
			d->mStroking = true;
			d->onStrokingOrToolEditingChanged();
			break;
		case QEvent::TabletRelease:
		case QEvent::MouseButtonRelease:
			d->mStroking = false;
			d->onStrokingOrToolEditingChanged();
			break;
		default:
			break;
	}
	return QWidget::event(event);
}

CanvasViewport::CanvasViewport(Canvas *canvas, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	using namespace std::placeholders;

	d->mSelf = this;
	d->mCanvas = canvas;

	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);

	// setup objects

	auto keyTracker = new KeyTracker(this);
	auto eventSender = new CanvasToolEventFilter(canvas, this);
	auto navigator = new CanvasNavigatorEventFilter(keyTracker, canvas, this);
	connect(navigator, SIGNAL(clicked()), this, SLOT(setFocus()));

	installEventFilter(eventSender);
	installEventFilter(navigator);
	installEventFilter(keyTracker);

	// setup scrollbars
	{
		auto sx = new VanishingScrollBar(Qt::Horizontal, this);
		auto sy = new VanishingScrollBar(Qt::Vertical, this);

		auto onScrollBarXChanged = [this](int x) {
			if (d->mState.retinaMode)
				x *= 2;
			d->mCanvas->setTranslationX(d->mCanvas->maxAbsoluteTranslation().x() - x);
		};

		auto onScrollBarYChanged = [this](int y) {
			if (d->mState.retinaMode)
				y *= 2;
			d->mCanvas->setTranslationY(d->mCanvas->maxAbsoluteTranslation().y() - y);
		};

		connect(sx, &VanishingScrollBar::sliderMoved, this, onScrollBarXChanged);
		connect(sy, &VanishingScrollBar::sliderMoved, this, onScrollBarYChanged);
		connect(sx, SIGNAL(valueChanged(int)), sy, SLOT(wakeUp()));
		connect(sy, SIGNAL(valueChanged(int)), sx, SLOT(wakeUp()));

		d->mScrollBarX = sx;
		d->mScrollBarY = sy;

		d->moveWidgets();
	}

	// connect to canvas
	{
		connect(canvas, &Canvas::transformsChanged, this, std::bind(&Data::setTransforms, d.data(), _1));
		d->setTransforms(canvas->transforms());

		auto onToolChanged = [this, eventSender](Tool *tool) {
			d->setTool(tool);
			eventSender->setTool(tool);
		};
		connect(canvas, &Canvas::toolChanged, this, onToolChanged);
		onToolChanged(canvas->tool());

		connect(this, &CanvasViewport::viewSizeChanged, canvas, &Canvas::setViewSize);
		canvas->setViewSize(this->viewSize());

		connect(canvas, &Canvas::retinaModeChanged, this, std::bind(&Data::setRetinaMode, d.data(), _1));
		d->setRetinaMode(canvas->isRetinaMode());

		connect(canvas->document(), &Document::sizeChanged, this, std::bind(&Data::setDocumentSize, d.data(), _1));
		d->setDocumentSize(canvas->document()->size());

		auto setFocusIfCanvasSame = [this](Canvas *canvas) {
			if (d->mCanvas == canvas)
				setFocus();
		};
		connect(canvas->workspace(), &Workspace::currentCanvasChanged, this, setFocusIfCanvasSame);
		setFocusIfCanvasSame(canvas->workspace()->currentCanvas());
	}
	connect(canvas->document()->layerScene(), &LayerScene::tilesUpdated, this, std::bind(&Data::updateTilesWithSet, d.data(), _1));
	d->updateTilesWithSet(canvas->document()->tileKeys());
}

CanvasViewport::~CanvasViewport()
{
}

} // namespace PaintField
