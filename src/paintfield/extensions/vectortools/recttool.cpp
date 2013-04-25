#include <QApplication>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QAction>
#include <QItemSelectionModel>

#include "paintfield/extensions/layerui/layeruicontroller.h"

#include "paintfield/core/textlayer.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/rectlayer.h"
#include "paintfield/core/layeritemmodel.h"

#include "recttool.h"

using namespace Malachite;

namespace PaintField {

static constexpr int handleRadius = 4;

class RectHandleItem : public QGraphicsItem
{
public:
	
	RectHandleItem(RectTool *tool, int handleTypes, QGraphicsItem *parent = 0) :
		QGraphicsItem(parent),
		m_tool(tool),
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
		
		QRect rect(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
		QRect innerRect = rect.adjusted(1,1,-1,-1);
		
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::black);
		painter->drawRect(rect);
		painter->setBrush(Qt::white);
		painter->drawRect(innerRect);
	}
	
protected:
	
	void mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		PAINTFIELD_DEBUG << event->pos();
		m_dragStartPos = event->scenePos();
		m_originalPos = pos();
	}
	
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		PAINTFIELD_DEBUG << event->pos();
		auto newPos = m_originalPos + (event->scenePos() - m_dragStartPos);
		m_tool->onHandleMoved(newPos, m_handleTypes);
	}
	
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{
		Q_UNUSED(event)
		m_tool->onHandleMoveFinished();
	}
	
private:
	
	RectTool *m_tool = 0;
	QPointF m_dragStartPos;
	QPointF m_originalPos;
	int m_handleTypes;
	int m_radius = handleRadius;
};

class RectInserter
{
public:
	
	RectInserter(RectTool::AddingType addingType, RectTool *tool) :
		m_addingType(addingType),
		m_tool(tool),
		m_scene(tool->layerScene())
	{}
	
	void press(const TabletInputData &input)
	{
		m_start = input.pos;
	}
	
	void move(const TabletInputData &input)
	{
		if (m_layerToAdd)
		{
			resize(input.pos);
		}
		else
		{
			auto diff = m_start = input.pos;
			if ( std::fabs(diff.x()) + std::fabs(diff.y()) >= qApp->startDragDistance() )
				start();
		}
	}
	
	void release()
	{
		if (m_layerToAdd)
			finish();
	}
	
private:
	
	void start()
	{
		auto createNew = [this]()
		{
			AbstractRectLayer *layer;
			
			if (m_addingType == RectTool::AddText)
			{
				auto textLayer = new TextLayer();
				textLayer->setText(QObject::tr("Text"));
				textLayer->setName(QObject::tr("Text"));
				
				layer = textLayer;
			}
			else
			{
				layer = new RectLayer();
				
				if (m_addingType == RectTool::AddEllipse)
				{
					layer->setShapeType(AbstractRectLayer::ShapeTypeEllipse);
					layer->setName(QObject::tr("Ellipse"));
				}
				else
				{
					layer->setShapeType(AbstractRectLayer::ShapeTypeRect);
					layer->setName(QObject::tr("Rectangle"));
				}
			}
			
			layer->setFillBrush(Color::fromRgbValue(0.5, 0.5, 0.5));
			layer->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
			
			return layer;
		};
		
		m_layerToAdd.reset(createNew());
		
		auto current = m_scene->current();
		if (current)
		{
			m_parent = current.parent();
			m_index = current.index();
		}
		else
		{
			m_parent = m_scene->rootLayer();
			m_index = 0;
		}
		
		m_tool->addLayerInsertion(m_parent, m_index, m_layerToAdd.data());
	}
	
	void resize(const Vec2D &pos)
	{
		auto offset = pos - m_start;
		
		auto oldBoundingRect = m_layerToAdd->boundingRect();
		m_layerToAdd->setRect(QRectF(m_start.x(), m_start.y(), offset.x(), offset.y()));
		emit m_tool->requestUpdate(Surface::rectToKeys((oldBoundingRect | m_layerToAdd->boundingRect()).toAlignedRect()));
	}
	
	void finish()
	{
		m_tool->clearLayerInsertions();
		m_scene->addLayers({m_layerToAdd.take()}, m_parent, m_index, QObject::tr("Add Rectangle"));
		m_scene->setCurrent(m_parent.child(m_index));
	}
	
	RectTool::AddingType m_addingType;
	
	QScopedPointer<AbstractRectLayer> m_layerToAdd;
	Vec2D m_start;
	LayerRef m_parent;
	int m_index;
	
	RectTool *m_tool;
	LayerScene *m_scene;
};

struct RectTool::Data
{
	LayerUIController *layerController = 0;
	
	AddingType addingType = NoAdding;
	
	QScopedPointer<RectInserter> inserter;
	
	QScopedPointer<AbstractRectLayer> rectLayer;
	LayerRef current;
	QHash<int, RectHandleItem *> handles;
	
	Mode mode = NoOperation;
	Vec2D dragStartPos;
	Vec2D originalRectPos;
	
	QPointSet rectKeys() const
	{
		auto rect = rectLayer->rect().adjusted(-handleRadius, -handleRadius, handleRadius, handleRadius);
		return Surface::rectToKeys(rect.toAlignedRect());
	}
};

RectTool::RectTool(AddingType type, Canvas *canvas) :
	Tool(canvas),
	d(new Data)
{
	d->layerController = canvas->findChild<LayerUIController *>();
	
	d->addingType = type;
	
	{
		auto group = new QGraphicsItemGroup();
		//group->setFiltersChildEvents(false);
		group->setHandlesChildEvents(false);
		setGraphicsItem(group);
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
	
	connect(layerScene(), SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(onCurrentChanged(LayerRef)));
	connect(layerScene(), SIGNAL(layerPropertyChanged(LayerRef)), this, SLOT(onLayerPropertyChanged(LayerRef)));
	connect(canvas->viewController(), SIGNAL(transformUpdated()), this, SLOT(moveHandles()));
	
	onCurrentChanged(layerScene()->current());
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
	// pass event to the graphics item
	if (graphicsItem()->scene()->itemAt(event->viewPos))
	{
		event->ignore();
		return;
	}
	
	auto layer = layerScene()->rootLayer()->descendantAt(event->data.pos.toQPoint(), handleRadius);
	
	// set clicked layer to current
	layerScene()->setCurrent(layer);
	
	if (d->rectLayer && layer)
		d->mode = Dragging;
	else if (d->addingType != NoAdding) // other than rect layer or no layer selected
		d->mode = Inserting;
	
	switch (d->mode)
	{
		case Dragging:
		{
			d->dragStartPos = event->data.pos;
			d->originalRectPos = d->rectLayer->rect().topLeft();
			break;
		}
		case Inserting:
		{
			d->inserter.reset(new RectInserter(d->addingType, this));
			d->inserter->press(event->data);
			break;
		}
		default:
			break;
	}
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	switch (d->mode)
	{
		case Dragging:
		{
			auto delta = event->data.pos - d->dragStartPos;
			auto originalKeys = d->rectKeys();
			d->rectLayer->setRect(QRectF(delta + d->originalRectPos, d->rectLayer->rect().size()));
			emit requestUpdate(originalKeys | d->rectKeys());
			moveHandles();
			break;
		}
		case Inserting:
		{
			d->inserter->move(event->data);
			break;
		}
		default:
			break;
	}
}

void RectTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	Q_UNUSED(event)
	
	switch (d->mode)
	{
		case Dragging:
		{
			onHandleMoveFinished();
			break;
		}
		case Inserting:
		{
			d->inserter->release();
			d->inserter.reset();
			break;
		}
		default:
			break;
	}
	
	d->mode = NoOperation;
}

void RectTool::onCurrentChanged(const LayerRef &layer)
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
			moveHandles();
			graphicsItem()->setVisible(true);
			addLayerDelegation(layer);
			
			return;
		}
	}
	
	d->rectLayer.reset();
	graphicsItem()->setVisible(false);
}

void RectTool::onLayerPropertyChanged(const LayerRef &layer)
{
	if (d->current == layer && d->rectLayer && layer->isType<AbstractRectLayer>())
	{
		d->rectLayer.reset(static_cast<AbstractRectLayer *>(layer->clone()));
	}
}

void RectTool::addHandle(int handleTypes, qreal zValue)
{
	auto handle = new RectHandleItem(this, handleTypes, graphicsItem());
	handle->setVisible(true);
	handle->setZValue(zValue);
	d->handles[handleTypes] = handle;
}

void RectTool::moveHandles()
{
	if (!d->rectLayer)
		return;
	
	auto rect = d->rectLayer->rect();
	
	auto transformToView = canvas()->viewController()->transformFromScene().toQTransform();
	auto topLeft = rect.topLeft() * transformToView;
	auto topRight = rect.topRight() * transformToView;
	auto bottomLeft = rect.bottomLeft() * transformToView;
	auto bottomRight = rect.bottomRight() * transformToView;
	
	PAINTFIELD_DEBUG << topLeft;
	
	d->handles[Left]->setPos( (topLeft + bottomLeft) * 0.5 );
	d->handles[Right]->setPos( (topRight + bottomRight) * 0.5 );
	d->handles[Top]->setPos( (topLeft + topRight) * 0.5 );
	d->handles[Bottom]->setPos( (bottomLeft + bottomRight) * 0.5 );
	
	d->handles[Left | Top]->setPos(topLeft);
	d->handles[Left | Bottom]->setPos(bottomLeft);
	d->handles[Right | Top]->setPos(topRight);
	d->handles[Right | Bottom]->setPos(bottomRight);
}

void RectTool::onHandleMoved(const QPointF &pos, int handleTypes)
{
	PAINTFIELD_DEBUG;
	
	if (!d->rectLayer)
		return;
	
	QPointSet oldKeys = d->rectKeys();
	
	auto rect = d->rectLayer->rect();
	auto scenePos = pos * canvas()->viewController()->transformToScene().toQTransform();
	
	if (handleTypes & Left)
		rect.setLeft(scenePos.x());
	
	if (handleTypes & Right)
		rect.setRight(scenePos.x());
	
	if (handleTypes & Top)
		rect.setTop(scenePos.y());
	
	if (handleTypes & Bottom)
		rect.setBottom(scenePos.y());
	
	d->rectLayer->setRect(rect);
	
	emit requestUpdate(d->rectKeys() | oldKeys);
	moveHandles();
}

void RectTool::onHandleMoveFinished()
{
	PAINTFIELD_DEBUG;
	
	if (!d->rectLayer)
		return;
	
	layerScene()->setLayerProperty(d->current, d->rectLayer->rect(), RoleRect, tr("Change Rect"));
}

} // namespace PaintField
