#include <QTimer>

#include "scopedtimer.h"
#include "application.h"
#include "settingsmanager.h"
#include "smartpointer.h"
#include "drawutil.h"
#include "tool.h"
#include "layerrenderer.h"
#include "canvas.h"
#include "appcontroller.h"
#include "workspace.h"
#include "keytracker.h"
#include "widgets/vanishingscrollbar.h"

#include "canvasviewport.h"

#include "canvasview.h"

namespace PaintField
{

using namespace Malachite;


class CanvasRenderer : public LayerRenderer
{
public:
	CanvasRenderer() : LayerRenderer() {}
	
	void setTool(Tool *tool) { _tool = tool; }
	
protected:
	
	void drawLayer(SurfacePainter *painter, const Layer *layer)
	{
		if (_tool && _tool->customDrawLayers().contains(layer))
			_tool->drawLayer(painter, layer);
		else
			LayerRenderer::drawLayer(painter, layer);
	}
	
private:
	
	Tool *_tool = 0;
};

struct Navigation
{
	double scale = 1, rotation = 0;
	QPoint translation;
};

class CanvasView::Data
{
public:
	
	Canvas *canvas = 0;
	Tool *tool = 0;
	
	double mousePressure = 0;
	bool tabletActive = false;
	
	KeyTracker *keyTracker = 0;
	
	QCursor toolCursor;
	
	QKeySequence scaleKeys, rotationKeys, translationKeys;
	
	DragNavigationMode dragNavigationMode = NoNavigation;
	QPoint navigationOrigin;
	
	Navigation nav, backupNav;
	bool mirrored = false, retinaMode = false;
	
	QSize sceneSize;
	QPoint viewCenter;
	
	Affine2D transformToScene, transformFromScene;
	
	QPointer<CanvasViewport> viewport = 0;
	VanishingScrollBar *scrollBarX = 0, *scrollBarY = 0;
	
	QTimer *accurateUpdateTimer = 0;
	
	QPoint maxAbsTranslation;
	
	bool updateEnabled = true;
};

CanvasView::CanvasView(Canvas *canvas, QWidget *parent) :
    QWidget(parent),
    d(new Data)
{
	d->canvas = canvas;
	d->sceneSize = canvas->document()->size();
	d->nav.translation = canvas->translation();
	d->nav.scale = canvas->scale();
	d->nav.rotation = canvas->rotation();
	
	d->keyTracker = new KeyTracker(this);
	
	d->viewCenter = QPoint(width() / 2, height() / 2);
	
	// setup scrollbars
	{
		d->scrollBarX = new VanishingScrollBar(Qt::Horizontal, this);
		d->scrollBarY = new VanishingScrollBar(Qt::Vertical, this);
		
		moveScrollBars();
		
		connect(d->scrollBarX, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarXChanged(int)));
		connect(d->scrollBarY, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarYChanged(int)));
	}
	
	// setup viewport
	{
		auto viewport = new CanvasViewport(window());
		d->viewport = viewport;
		moveViewport();
		
		{
			auto timer = new QTimer(this);
			timer->setSingleShot(true);
			timer->setInterval(100);
			connect(timer, SIGNAL(timeout()), this, SLOT(onViewportAccurateUpdate()));
			d->accurateUpdateTimer = timer;
		}
		
		viewport->setDocumentSize(d->sceneSize);
		updateTransforms();
		updateTiles(layerModel()->document()->tileKeys());
	}
	
	// connect to canvas
	{
		connect(canvas, SIGNAL(scaleChanged(double)), this, SLOT(setScale(double)));
		connect(canvas, SIGNAL(rotationChanged(double)), this, SLOT(setRotation(double)));
		connect(canvas, SIGNAL(translationChanged(QPoint)), this, SLOT(setTranslation(QPoint)));
		connect(canvas, SIGNAL(mirroredChanged(bool)), this, SLOT(setMirrored(bool)));
		connect(canvas, SIGNAL(retinaModeChanged(bool)), this, SLOT(setRetinaMode(bool)));
		
		setScale(canvas->scale());
		setRotation(canvas->rotation());
		setTranslation(canvas->translation());
		
		connect(canvas, SIGNAL(toolChanged(Tool*)), this, SLOT(setTool(Tool*)));
		setTool(canvas->tool());
		
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)), this, SLOT(deleteLater()));
		
		canvas->setView(this);
	}
	
	connect(layerModel(), SIGNAL(tilesUpdated(QPointSet)),
	        this, SLOT(updateTiles(QPointSet)));
	
	connect(appController()->app(), SIGNAL(tabletActiveChanged(bool)),
	        this, SLOT(onTabletActiveChanged(bool)));
	onTabletActiveChanged(appController()->app()->isTabletActive());
	
	// setup key bindings
	{
		auto keyBindingHash = appController()->settingsManager()->settings()[".key-bindings"].toMap();
		
		d->translationKeys = keyBindingHash["paintfield.canvas.dragTranslation"].toString();
		d->scaleKeys = keyBindingHash["paintfield.canvas.dragScale"].toString();
		d->rotationKeys = keyBindingHash["paintfield.canvas.dragRotation"].toString();
	}
	
	// set widget properties
	{
		setMouseTracking(true);
	}
}

CanvasView::~CanvasView()
{
	if (d->viewport)
		d->viewport->deleteLater();
	delete d;
}

bool CanvasView::isUpdateTilesEnabled() const { return d->updateEnabled; }

void CanvasView::setUpdateTilesEnabled(bool enable)
{
	d->updateEnabled = enable;
}

QPoint CanvasView::viewCenter() const { return d->viewCenter; }

void CanvasView::setScale(double value)
{
	d->nav.scale = value;
	updateTransforms();
}

void CanvasView::setRotation(double value)
{
	d->nav.rotation = value;
	updateTransforms();
}

void CanvasView::setTranslation(const QPoint &value)
{
	d->nav.translation = value;
	updateTransforms();
}

void CanvasView::setMirrored(bool mirrored)
{
	d->mirrored = mirrored;
	updateTransforms();
}

void CanvasView::setRetinaMode(bool mode)
{
	d->retinaMode = mode;
	updateTransforms();
}

Affine2D CanvasView::transformToScene() const { return d->transformToScene; }
Affine2D CanvasView::transformFromScene() const { return d->transformFromScene; }

void CanvasView::updateTransforms()
{
	QPoint sceneOffset = QPoint(d->sceneSize.width(), d->sceneSize.height()) / 2;
	QPoint viewOffset = viewCenter() + d->nav.translation;
	
	double scale = d->retinaMode ? d->nav.scale * 0.5 : d->nav.scale;
	
	auto transform = Affine2D::fromTranslation(Vec2D(viewOffset)) *
	                 Affine2D::fromRotationDegrees(d->nav.rotation) *
	                 Affine2D::fromScale(scale) *
	                 Affine2D::fromTranslation(Vec2D(-sceneOffset));
	
	if (d->mirrored)
		transform = transform * Affine2D(-1, 0, 0, 1, d->sceneSize.width(), 0);
	
	d->transformFromScene = transform;
	d->transformToScene = transform.inverted();
	d->viewport->setTransform(transform, d->nav.translation, scale, d->nav.rotation, d->retinaMode);
	
	updateScrollBarRange();
	updateScrollBarValue();
	
	d->accurateUpdateTimer->start();
	
	d->viewport->update();
}

Canvas *CanvasView::canvas() { return d->canvas; }
Document *CanvasView::document() { return d->canvas->document(); }
LayerModel *CanvasView::layerModel() { return d->canvas->layerModel(); }

void CanvasView::setTool(Tool *tool)
{
	if (tool)
	{
		d->tool = tool;
		
		connect(tool, SIGNAL(requestUpdate(QPointSet)), this, SLOT(updateTiles(QPointSet)));
		connect(tool, SIGNAL(requestUpdate(QHash<QPoint,QRect>)), this, SLOT(updateTiles(QHash<QPoint,QRect>)));
		
		d->toolCursor = tool->cursor();
		setCursor(tool->cursor());
	}
}

void CanvasView::updateTiles(const QPointSet &keys, const QHash<QPoint, QRect> &rects)
{
	if (!d->updateEnabled)
		return;
	
	d->viewport->beforeUpdateTile(CanvasViewport::PartialAccurateUpdate);
	
	CanvasRenderer renderer;
	renderer.setTool(d->tool);
	
	Surface surface = renderer.renderToSurface(layerModel()->rootLayer()->children(), keys, rects);
	
	static const Pixel whitePixel = Color::fromRgbValue(1,1,1).toPixel();
	
	auto updateTile = [this, &surface](const QPoint &key, const QRect &rect)
	{
		Image image(rect.size());
		image.fill(whitePixel);
		
		if (surface.contains(key))
		{
			Painter painter(&image);
			painter.drawPreTransformedImage(-rect.topLeft(), surface.tile(key));
		}
		
		d->viewport->updateTile(key, image, rect.topLeft());
	};
	
	if (rects.isEmpty())
	{
		for (const QPoint &key : keys)
		{
			auto rect = QRect(0, 0, Surface::tileWidth(), Surface::tileWidth());
			updateTile(key, rect);
		}
	}
	else
	{
		for (auto iter = rects.begin(); iter != rects.end(); ++iter)
		{
			updateTile(iter.key(), iter.value());
		}
	}
	
	d->viewport->afterUpdateTile();
}

void CanvasView::onClicked()
{
	setFocus();
	canvas()->workspace()->setCurrentCanvas(canvas());
}

void CanvasView::onTabletActiveChanged(bool active)
{
	d->tabletActive = active;
}

void CanvasView::onViewportAccurateUpdate()
{
	d->viewport->updateWholeAccurately();
}

void CanvasView::onScrollBarXChanged(int value)
{
	d->canvas->setTranslationX(d->maxAbsTranslation.x() - value);
}

void CanvasView::onScrollBarYChanged(int value)
{
	d->canvas->setTranslationY(d->maxAbsTranslation.y() - value);
}

void CanvasView::updateScrollBarRange()
{
	int radius = ceil(hypot(d->sceneSize.width(), d->sceneSize.height()) * d->nav.scale * 0.5);
	
	d->maxAbsTranslation = QPoint(radius + this->width(), radius + this->height());
	
	d->scrollBarX->setRange(0, 2 * d->maxAbsTranslation.x());
	d->scrollBarY->setRange(0, 2 * d->maxAbsTranslation.y());
	
	d->scrollBarX->setPageStep(this->width());
	d->scrollBarY->setPageStep(this->height());
}

void CanvasView::updateScrollBarValue()
{
	d->scrollBarX->setValue(d->maxAbsTranslation.x() - d->nav.translation.x());
	d->scrollBarY->setValue(d->maxAbsTranslation.y() - d->nav.translation.y());
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
	if (d->tool)
		d->tool->toolEvent(event);
	
	//PAINTFIELD_DEBUG << "pressed:" << event->key() << "modifiers" << event->modifiers() << "at" << this;
	d->keyTracker->pressKey(event->key());
	d->keyTracker->setModifiers(event->modifiers());
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
	if (d->tool)
		d->tool->toolEvent(event);
	
	//PAINTFIELD_DEBUG << "released:" << event->key() << "modifiers" << event->modifiers() << "at" << this;
	d->keyTracker->releaseKey(event->key());
}

void CanvasView::focusInEvent(QFocusEvent *)
{
	d->viewport->updateWholeAccurately();
}

void CanvasView::enterEvent(QEvent *e)
{
	super::enterEvent(e);
	setFocus();
	//qApp->setOverrideCursor(d->toolCursor);
}

void CanvasView::leaveEvent(QEvent *e)
{
	super::leaveEvent(e);
	
	//while (qApp->overrideCursor())
	//	qApp->restoreOverrideCursor();
}

void CanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (d->tool)
		event->setAccepted(sendCanvasMouseEvent(event));
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
	onClicked();
	
	if (event->button() == Qt::LeftButton)
	{
		if (tryBeginDragNavigation(event->pos()))
		{
			event->accept();
			return;
		}
	}
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
	if (continueDragNavigation(event->pos()))
	{
		event->accept();
		return;
	}
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	endDragNavigation();
	
	if (d->tool)
		event->setAccepted(sendCanvasTabletEvent(event) || sendCanvasMouseEvent(event));
}

void CanvasView::tabletEvent(QTabletEvent *event)
{
	auto toNewEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::TabletMove:
				return PaintField::EventWidgetTabletMove;
			case QEvent::TabletPress:
				return PaintField::EventWidgetTabletPress;
			case QEvent::TabletRelease:
				return PaintField::EventWidgetTabletRelease;
		}
	};
	
	switch (event->type())
	{
		case QEvent::TabletPress:
			
			onClicked();
			if (tryBeginDragNavigation(event->pos()))
			{
				event->accept();
				return;
			}
			break;
			
		case QEvent::TabletMove:
			
			if (continueDragNavigation(event->pos()))
			{
				event->accept();
				return;
			}
			break;
			
		case QEvent::TabletRelease:
			
			endDragNavigation();
			break;
			
		default:
			break;
	}
	
	TabletInputData data(event->hiResGlobalPos(), event->pressure(), event->rotation(), event->tangentialPressure(), Vec2D(event->xTilt(), event->yTilt()));
	WidgetTabletEvent widgetTabletEvent(toNewEventType(event->type()), event->globalPos(), event->pos(), data, event->modifiers());
	
	customTabletEvent(&widgetTabletEvent);
	event->setAccepted(widgetTabletEvent.isAccepted());
}

void CanvasView::customTabletEvent(WidgetTabletEvent *event)
{
	
	switch (int(event->type()))
	{
		case EventWidgetTabletPress:
			
			onClicked();
			if (tryBeginDragNavigation(event->posInt))
			{
				event->accept();
				return;
			}
			break;
			
		case EventWidgetTabletMove:
			
			if (continueDragNavigation(event->posInt))
			{
				event->accept();
				return;
			}
			break;
			
		case EventWidgetTabletRelease:
			
			endDragNavigation();
			break;
			
		default:
			break;
	}
	
	if (d->tool)
	{
		event->setAccepted(sendCanvasTabletEvent(event));
	}
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
	QPoint translation = d->canvas->translation();
	
	if (event->orientation() == Qt::Horizontal)
		translation += QPoint(event->delta(), 0);
	else
		translation += QPoint(0, event->delta());
	
	d->canvas->setTranslation(translation);
}

void CanvasView::resizeEvent(QResizeEvent *)
{
	moveScrollBars();
	moveViewport();
	d->viewCenter = QPoint(width() / 2, height() / 2);
	updateTransforms();
}

void CanvasView::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::ParentChange:
			moveViewport();
			break;
		case QEvent::EnabledChange:
			d->viewport->setEnabled(isEnabled());
			break;
		default:
			break;
	}
}

void CanvasView::showEvent(QShowEvent *)
{
	d->viewport->show();
}

void CanvasView::hideEvent(QHideEvent *)
{
	d->viewport->hide();
}

bool CanvasView::event(QEvent *event)
{
	switch ((int)event->type())
	{
		case EventWidgetTabletMove:
		case EventWidgetTabletPress:
		case EventWidgetTabletRelease:
			customTabletEvent(static_cast<WidgetTabletEvent *>(event));
			return event->isAccepted();
		default:
			return QWidget::event(event);
	}
}

bool CanvasView::sendCanvasMouseEvent(QMouseEvent *event)
{
	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::MouseMove:
				return EventCanvasMouseMove;
			case QEvent::MouseButtonPress:
				return EventCanvasMousePress;
			case QEvent::MouseButtonRelease:
				return EventCanvasMouseRelease;
			case QEvent::MouseButtonDblClick:
				return EventCanvasMouseDoubleClick;
		}
	};
	
	CanvasMouseEvent canvasEvent(toCanvasEventType(event->type()), event->globalPos(), transformToScene() * event->posF(), event->modifiers());
	d->tool->toolEvent(&canvasEvent);
	
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(WidgetTabletEvent *event)
{
	TabletInputData data = event->globalData;
	Vec2D globalPos = data.pos;
	data.pos += Vec2D(event->posInt - event->globalPosInt);
	data.pos = transformToScene() * data.pos;
	
	auto toCanvasEventType = [](int type)
	{
		switch (type)
		{
			default:
			case EventWidgetTabletMove:
				return EventCanvasTabletMove;
			case EventWidgetTabletPress:
				return EventCanvasTabletPress;
			case EventWidgetTabletRelease:
				return EventCanvasTabletRelease;
		}
	};
	
	CanvasTabletEvent canvasEvent(toCanvasEventType(event->type()), globalPos, event->globalPosInt, data, event->modifiers());
	d->tool->toolEvent(&canvasEvent);
	
	return canvasEvent.isAccepted();
}

bool CanvasView::sendCanvasTabletEvent(QMouseEvent *mouseEvent)
{
	auto toCanvasEventType = [](QEvent::Type type)
	{
		switch (type)
		{
			default:
			case QEvent::MouseMove:
				return EventCanvasTabletMove;
			case QEvent::MouseButtonPress:
				return EventCanvasTabletPress;
			case QEvent::MouseButtonRelease:
				return EventCanvasTabletRelease;
		}
	};
	
	int type = toCanvasEventType(mouseEvent->type());
	
	if (type == EventCanvasTabletPress)
		d->mousePressure = 1.0;
	if (type == EventCanvasTabletRelease)
		d->mousePressure = 0.0;
	
	TabletInputData data(transformToScene() * mouseEvent->posF(), d->mousePressure, 0, 0, Vec2D(0));
	CanvasTabletEvent tabletEvent(type, mouseEvent->globalPos(), mouseEvent->globalPos(), data, mouseEvent->modifiers());
	d->tool->toolEvent(&tabletEvent);
	return tabletEvent.isAccepted();
}

bool CanvasView::tryBeginDragNavigation(const QPoint &pos)
{
	if (d->keyTracker->match(d->scaleKeys))
	{
		beginDragScaling(pos);
		return true;
	}
	if (d->keyTracker->match(d->rotationKeys))
	{
		beginDragRotation(pos);
		return true;
	}
	 if (d->keyTracker->match(d->translationKeys))
	{
		beginDragTranslation(pos);
		return true;
	}
	
	return false;
}

bool CanvasView::continueDragNavigation(const QPoint &pos)
{
	switch (d->dragNavigationMode)
	{
		default:
		case NoNavigation:
			return false;
		case Translating:
			continueDragTranslation(pos);
			return true;
		case Scaling:
			continueDragScaling(pos);
			return true;
		case Rotating:
			continueDragRotation(pos);
			return true;
	}
}

void CanvasView::endDragNavigation()
{
	endDragTranslation();
	endDragScaling();
	endDragRotation();
}

void CanvasView::beginDragTranslation(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::ClosedHandCursor);
	
	d->dragNavigationMode = Translating;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragTranslation(const QPoint &pos)
{
	d->canvas->setTranslation(d->backupNav.translation + (pos - d->navigationOrigin));
}

void CanvasView::endDragTranslation()
{
	qApp->restoreOverrideCursor();
	d->dragNavigationMode = NoNavigation;
}

void CanvasView::beginDragScaling(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::SizeVerCursor);
	
	d->dragNavigationMode = Scaling;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragScaling(const QPoint &pos)
{
	auto delta = pos - d->navigationOrigin;
	
	constexpr double divisor = 100;
	
	double scaleRatio = exp2(-delta.y() / divisor);
	double scale = d->backupNav.scale * scaleRatio;
	
	auto navigationOffset = d->navigationOrigin - viewCenter();
	
	auto translation = (d->backupNav.translation - navigationOffset) * scaleRatio + navigationOffset;
	
	d->canvas->setScale(scale);
	d->canvas->setTranslation(translation);
}

void CanvasView::endDragScaling()
{
	qApp->restoreOverrideCursor();
	d->dragNavigationMode = NoNavigation;
}

void CanvasView::beginDragRotation(const QPoint &pos)
{
	qApp->setOverrideCursor(Qt::ClosedHandCursor);
	
	d->dragNavigationMode = Rotating;
	d->navigationOrigin = pos;
	d->backupNav = d->nav;
}

void CanvasView::continueDragRotation(const QPoint &pos)
{
	auto originalDelta = d->navigationOrigin - viewCenter();
	auto delta = pos - viewCenter();
	if (originalDelta != QPoint() && delta != QPoint())
	{
		auto originalRotation = atan2(originalDelta.y(), originalDelta.x()) / M_PI * 180.0;
		auto deltaRotation = atan2(delta.y(), delta.x()) / M_PI * 180.0;
		
		QTransform transform;
		transform.rotate(deltaRotation - originalRotation);
		
		auto translation = d->backupNav.translation * transform;
		d->canvas->setRotation(d->backupNav.rotation + deltaRotation - originalRotation);
		d->canvas->setTranslation(translation);
	}
}

void CanvasView::endDragRotation()
{
	qApp->restoreOverrideCursor();
	d->dragNavigationMode = NoNavigation;
}

void CanvasView::moveViewport()
{
	PAINTFIELD_DEBUG << "moving viewport";
	
	d->viewport->setParent(window());
	QRect geom(Util::mapToWindow(this, QPoint()), this->geometry().size());
	d->viewport->setGeometry(geom);
	d->viewport->show();
	d->viewport->lower();
}

void CanvasView::moveScrollBars()
{
	int barWidthX = d->scrollBarX->totalBarWidth();
	int barWidthY = d->scrollBarY->totalBarWidth();
	
	auto widgetRect = QRect(QPoint(), geometry().size());
	
	auto scrollBarXRect = widgetRect.adjusted(0, widgetRect.height() - barWidthY, -barWidthX, 0);
	auto scrollBarYRect = widgetRect.adjusted(widgetRect.width() - barWidthX, 0, 0, -barWidthY);
	
	d->scrollBarX->setGeometry(scrollBarXRect);
	d->scrollBarY->setGeometry(scrollBarYRect);
}


}
