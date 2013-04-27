#include <QApplication>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QAction>
#include <QItemSelectionModel>

#include <functional>
#include <boost/signals2.hpp>

#include "paintfield/extensions/layerui/layeruicontroller.h"

#include "paintfield/core/textlayer.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/rectlayer.h"
#include "paintfield/core/layeritemmodel.h"

#include "recttool.h"

using namespace Malachite;

namespace PaintField {

static constexpr int handleRadius = 4;

class FrameGraphicsItem : public QGraphicsItem
{
public:
	
	FrameGraphicsItem(QGraphicsItem *parent = 0) :
		QGraphicsItem(parent)
	{}
	
	void setPath(const QPainterPath &path)
	{
		m_path = path;
	}
	
	QPainterPath path() const { return m_path; }
	
	QRectF boundingRect() const override
	{
		return m_path.boundingRect();
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
	{
		QPen pen;
		pen.setWidth(1);
		pen.setColor(QColor(128,128,128,128));
		
		painter->save();
		
		painter->setRenderHint(QPainter::Antialiasing, false);
		painter->setCompositionMode(QPainter::CompositionMode_Difference);
		painter->setBrush(Qt::NoBrush);
		painter->setPen(pen);
		
		painter->drawPath(m_path);
		
		painter->restore();
	}
private:
	
	QPainterPath m_path;
};

enum HandleType
{
	Left = 1,
	Right = 1 << 1,
	Top = 1 << 2,
	Bottom = 1 << 3
};

class RectHandleItem : public QGraphicsItem
{
public:
	
	RectHandleItem(int handleTypes, QGraphicsItem *parent = 0) :
		QGraphicsItem(parent),
		m_handleTypes(handleTypes)
	{
	}
	
	QRectF boundingRect() const override
	{
		return QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
	{
		Q_UNUSED(option)
		Q_UNUSED(widget)
		
		if (m_transparent)
			return;
		
		QRect rect(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
		QRect innerRect = rect.adjusted(1,1,-1,-1);
		
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::black);
		painter->drawRect(rect);
		painter->setBrush(Qt::white);
		painter->drawRect(innerRect);
	}
	
	/**
	 * @return A signal which emits the new position and the handle type when the item is moved
	 */
	boost::signals2::signal<void (const QPointF &, int)> &signalOnHandleMoved() { return m_signalOnHandleMoved; }
	
	boost::signals2::signal<void ()> &signalOnHandleMoveFinished() { return m_signalOnHandleMoveFinished; }
	
	int handleTypes() const { return m_handleTypes; }
	
	void invertHandleTypeLeftRight()
	{
		if (m_handleTypes & Left)
		{
			m_handleTypes &= ~Left;
			m_handleTypes |= Right;
		}
		else if (m_handleTypes & Right)
		{
			m_handleTypes &= ~Right;
			m_handleTypes |= Left;
		}
	}
	
	void invertHandleTypeTopBottom()
	{
		if (m_handleTypes & Top)
		{
			m_handleTypes &= ~Top;
			m_handleTypes |= Bottom;
		}
		else if (m_handleTypes & Bottom)
		{
			m_handleTypes &= ~Bottom;
			m_handleTypes |= Top;
		}
	}
	
	bool isTransparent() const { return m_transparent; }
	void setTransparent(bool x)
	{
		m_transparent = x;
		update();
	}
	
protected:
	
	void mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		m_dragStartPos = event->scenePos();
		m_originalPos = pos();
	}
	
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		auto newPos = m_originalPos + (event->scenePos() - m_dragStartPos);
		m_signalOnHandleMoved(newPos, m_handleTypes);
	}
	
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{
		Q_UNUSED(event)
		m_signalOnHandleMoveFinished();
	}
	
private:
	
	QPointF m_dragStartPos;
	QPointF m_originalPos;
	int m_handleTypes;
	int m_radius = handleRadius;
	boost::signals2::signal<void (const QPointF &, int)> m_signalOnHandleMoved;
	boost::signals2::signal<void ()> m_signalOnHandleMoveFinished;
	bool m_transparent = false;
};

struct RectTool::Data
{
	LayerUIController *layerController = 0;
	
	AddingType addingType = NoAdding;
	
	QScopedPointer<AbstractRectLayer> rectLayer;
	LayerRef current;
	QList<RectHandleItem *> handles;
	
	RectHandleItem *findHandle(int types)
	{
		for (auto handle : handles)
		{
			if (handle->handleTypes() == types)
				return handle;
		}
		return 0;
	}
	
	FrameGraphicsItem *frameItem = 0;
	
	Mode mode = NoOperation;
	bool dragDistanceEnough = false;
	Vec2D dragStartPos;
	Vec2D originalRectPos;
	
	QPointSet rectKeys() const
	{
		auto rect = rectLayer->rect().adjusted(-handleRadius, -handleRadius, handleRadius, handleRadius);
		return Surface::rectToKeys(rect.toAlignedRect());
	}
	
	LayerRef addingParent;
	int addingIndex;
};

RectTool::RectTool(AddingType type, Canvas *canvas) :
	Tool(canvas),
	d(new Data)
{
	d->layerController = canvas->findChild<LayerUIController *>();
	
	d->addingType = type;
	
	{
		auto group = new QGraphicsItemGroup();
		group->setHandlesChildEvents(false);
		setGraphicsItem(group);
		
		{
			auto frame = new FrameGraphicsItem(group);
			d->frameItem = frame;
		}
	}
	
	addHandle(Top | Left, 1);
	addHandle(Top | Right, 1);
	addHandle(Bottom | Left, 1);
	addHandle(Bottom | Right, 1);
	addHandle(Top, 0);
	addHandle(Bottom, 0);
	addHandle(Left, 0);
	addHandle(Right, 0);
	
	graphicsItem()->setVisible(false);
	
	connect(layerScene(), SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(updateCurrent(LayerRef)));
	connect(layerScene(), SIGNAL(layerPropertyChanged(LayerRef)), this, SLOT(updateCurrent(LayerRef)));
	connect(canvas->viewController(), SIGNAL(transformUpdated()), this, SLOT(updateHandles()));
	
	updateCurrent(layerScene()->current());
}

RectTool::~RectTool()
{
	delete d;
}

void RectTool::drawLayer(SurfacePainter *painter, const Layer *layer)
{
	if (layer == d->current.pointer())
	{
		d->rectLayer->render(painter);
	}
}

void RectTool::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Backspace)
	{
		d->layerController->action(LayerUIController::ActionRemove)->trigger();
	}
}

void RectTool::tabletPressEvent(CanvasTabletEvent *event)
{
	if (d->mode != NoOperation)
		return;
	
	// pass event to the graphics item
	
	auto item = graphicsItem()->scene()->itemAt(event->viewPos);
	
	if (item && item != d->frameItem)
	{
		event->ignore();
		return;
	}
	
	auto layer = layerScene()->rootLayer()->descendantAt(event->data.pos.toQPoint(), handleRadius);
	
	// set clicked layer to current
	
	layerScene()->setCurrent(layer);
	
	// decide mode
	
	if (d->rectLayer && layer)
		d->mode = Dragging;
	else if (d->addingType != NoAdding) // other than rect layer or no layer selected
		d->mode = Inserting;
	
	if (d->mode == NoOperation)
		return;
	
	d->dragDistanceEnough = false;
	d->dragStartPos = event->data.pos;
	
	if (d->mode == Dragging)
		d->originalRectPos = d->rectLayer->rect().topLeft();
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	auto delta = event->data.pos - d->dragStartPos;
	
	if ( d->dragDistanceEnough )
	{
		switch (d->mode)
		{
			default:
			{
				break;
			}
			case Dragging:
			case Inserting:
			{
				auto originalKeys = d->rectKeys();
				
				QRectF rect;
				
				if (d->mode == Dragging)
				{
					// move rect
					rect = QRectF(delta + d->originalRectPos, d->rectLayer->rect().size());
				}
				else
				{
					// set a rect which is spreaded by dragStartPos and event pos
					
					auto p1 = d->dragStartPos, p2 = event->data.pos;
					rect.setLeft(std::min(p1.x(), p2.x()));
					rect.setRight(std::max(p1.x(), p2.x()));
					rect.setTop(std::min(p1.y(), p2.y()));
					rect.setBottom(std::max(p1.y(), p2.y()));
				}
				
				d->rectLayer->setRect(rect);
				
				emit requestUpdate(originalKeys | d->rectKeys());
				updateHandles();
				break;
			}
		}
	}
	else
	{
		if ( ( std::abs( delta.x() ) + std::abs( delta.y() ) ) >= qApp->startDragDistance() )
		{
			d->dragDistanceEnough = true;
			
			if (d->mode == Inserting)
				startAdding();
		}
		else
			return;
	}
}

void RectTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	Q_UNUSED(event)
	
	if (d->dragDistanceEnough)
	{
		switch (d->mode)
		{
			case Dragging:
			{
				commit();
				break;
			}
			case Inserting:
			{
				finishAdding();
				break;
			}
			default:
				break;
		}
	}
	
	d->mode = NoOperation;
	updateHandles();
}

void RectTool::updateCurrent(const LayerRef &layer)
{
	clearLayerDelegation();
	
	d->current = layer;
	
	if (layer && layer->isGraphicallySelectable())
		layerScene()->setSelection({layer});
	else
		layerScene()->setSelection({});
	
	if (layer)
	{
		auto rectLayer = dynamic_cast<const AbstractRectLayer *>(layer.pointer());
		if (rectLayer)
		{
			d->rectLayer.reset(static_cast<AbstractRectLayer *>(rectLayer->clone()));
			addLayerDelegation(layer);
			
			updateHandles();
			return;
		}
	}
	
	d->rectLayer.reset();
	updateHandles();
}

void RectTool::addHandle(int handleTypes, qreal zValue)
{
	auto handle = new RectHandleItem(handleTypes, graphicsItem());
	
	handle->setVisible(true);
	handle->setZValue(zValue);
	d->handles << handle;
	
	handle->signalOnHandleMoved().connect(std::bind(&RectTool::onHandleMoved, this, std::placeholders::_1, std::placeholders::_2));
	handle->signalOnHandleMoveFinished().connect(std::bind(&RectTool::onHandleMoveFinished, this));
}

void RectTool::updateHandles()
{
	graphicsItem()->setVisible(d->rectLayer);
	
	if (!d->rectLayer)
		return;
	
	auto rect = d->rectLayer->rect();
	
	// get vertices in scene coordinates
	auto transformToView = canvas()->viewController()->transformFromScene().toQTransform();
	auto topLeft = rect.topLeft() * transformToView;
	auto topRight = rect.topRight() * transformToView;
	auto bottomLeft = rect.bottomLeft() * transformToView;
	auto bottomRight = rect.bottomRight() * transformToView;
	
	PAINTFIELD_DEBUG << topLeft;
	
	d->findHandle(Left)->setPos( (topLeft + bottomLeft) * 0.5 );
	d->findHandle(Right)->setPos( (topRight + bottomRight) * 0.5 );
	d->findHandle(Top)->setPos( (topLeft + topRight) * 0.5 );
	d->findHandle(Bottom)->setPos( (bottomLeft + bottomRight) * 0.5 );
	
	d->findHandle(Left | Top)->setPos(topLeft);
	d->findHandle(Left | Bottom)->setPos(bottomLeft);
	d->findHandle(Right | Top)->setPos(topRight);
	d->findHandle(Right | Bottom)->setPos(bottomRight);
	
	for (auto handle : d->handles)
		handle->setTransparent(d->mode != NoOperation);
	
	if (d->rectLayer->isType<TextLayer>())
	{
		d->frameItem->setVisible(true);
		
		QPainterPath path;
		path.addRect(rect);
		
		d->frameItem->setPath(path * transformToView);
	}
	else
	{
		d->frameItem->setVisible(false);
	}
}

void RectTool::onHandleMoved(const QPointF &pos, int handleTypes)
{
	if (!d->rectLayer)
		return;
	
	d->mode = MovingHandle;
	
	QPointSet oldKeys = d->rectKeys();
	
	auto rect = d->rectLayer->rect();
	auto scenePos = pos * canvas()->viewController()->transformToScene().toQTransform();
	
	double left = rect.left();
	double right = rect.right();
	double top = rect.top();
	double bottom = rect.bottom();
	
	if (handleTypes & Left)
		left = scenePos.x();
	
	if (handleTypes & Right)
		right = scenePos.x();
	
	if (handleTypes & Top)
		top = scenePos.y();
	
	if (handleTypes & Bottom)
		bottom = scenePos.y();
	
	if (right < left)
	{
		std::swap(left, right);
		
		for (RectHandleItem *handle : d->handles)
			handle->invertHandleTypeLeftRight();
	}
	
	if (bottom < top)
	{
		std::swap(top, bottom);
		
		for (RectHandleItem *handle : d->handles)
			handle->invertHandleTypeTopBottom();
	}
	
	rect.setCoords(left, top, right, bottom);
	
	d->rectLayer->setRect(rect);
	
	emit requestUpdate(d->rectKeys() | oldKeys);
	updateHandles();
}

void RectTool::onHandleMoveFinished()
{
	d->mode = NoOperation;
	updateHandles();
	commit();
}

void RectTool::commit()
{
	if (d->rectLayer && d->current)
		layerScene()->setLayerProperty(d->current, d->rectLayer->rect(), RoleRect, tr("Change Rect"));
}

void RectTool::startAdding()
{
	auto createNew = [this]()
	{
		AbstractRectLayer *layer;
		
		if (d->addingType == RectTool::AddText)
		{
			auto textLayer = new TextLayer();
			textLayer->setText(tr("Text"));
			textLayer->setName(tr("Text"));
			
			layer = textLayer;
			
			QFont font;
			font.setPointSize(36);
			
			layer->setFillBrush(Color::fromRgbValue(0, 0, 0));
			layer->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
			layer->setStrokeEnabled(false);
			textLayer->setFont(font);
		}
		else
		{
			layer = new RectLayer();
			
			if (d->addingType == RectTool::AddEllipse)
			{
				layer->setShapeType(AbstractRectLayer::ShapeTypeEllipse);
				layer->setName(tr("Ellipse"));
			}
			else
			{
				layer->setShapeType(AbstractRectLayer::ShapeTypeRect);
				layer->setName(tr("Rectangle"));
			}
			
			layer->setFillBrush(Color::fromRgbValue(0.5, 0.5, 0.5));
			layer->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
		}
		
		return layer;
	};
	
	d->rectLayer.reset(createNew());
	
	if (d->current)
	{
		d->addingParent = d->current.parent();
		d->addingIndex = d->current.index();
	}
	else
	{
		d->addingParent = layerScene()->rootLayer();
		d->addingIndex = 0;
	}
	
	addLayerInsertion(d->addingParent, d->addingIndex, d->rectLayer.data());
}

void RectTool::finishAdding()
{
	PAINTFIELD_DEBUG;
	
	clearLayerInsertions();
	layerScene()->addLayers({d->rectLayer->clone()}, d->addingParent, d->addingIndex, tr("Add Rectangle"));
	layerScene()->setCurrent(d->addingParent.child(d->addingIndex));
}

} // namespace PaintField
