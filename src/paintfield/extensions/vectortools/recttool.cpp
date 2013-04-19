#include <QApplication>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "paintfield/core/layerscene.h"
#include "paintfield/core/rectlayer.h"

#include "recttool.h"

using namespace Malachite;

namespace PaintField {

class RectHandleItem : public QGraphicsPathItem
{
public:
	
	RectHandleItem(RectTool *tool, int handleTypes, QGraphicsItem *parent = 0) :
		QGraphicsPathItem(parent),
		m_tool(tool),
		m_handleTypes(handleTypes)
	{
		QPainterPath path;
		path.addEllipse(QPointF(), 5, 5);
		//setPen(Qt::NoPen);
		setBrush(Qt::white);
		setPath(path);
		
		PAINTFIELD_DEBUG << acceptedMouseButtons();
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
};

class RectInserter
{
public:
	
	RectInserter(RectTool *tool) :
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
		m_layerToAdd.reset(new RectLayer);
		m_layerToAdd->setName(QObject::tr("Rectangle"));
		m_layerToAdd->setFillBrush(Color::fromRgbValue(0.5, 0.5, 0.5));
		m_layerToAdd->setStrokeBrush(Color::fromRgbValue(0, 0, 0));
		
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
	
	QScopedPointer<RectLayer> m_layerToAdd;
	Vec2D m_start;
	LayerRef m_parent;
	int m_index;
	
	RectTool *m_tool;
	LayerScene *m_scene;
};

struct RectTool::Data
{
	QScopedPointer<RectInserter> inserter;
	
	QScopedPointer<RectLayer> rectLayer;
	LayerRef current;
	QHash<int, RectHandleItem *> handles;
};

RectTool::RectTool(Canvas *canvas) :
	Tool(canvas),
	d(new Data)
{
	{
		auto group = new QGraphicsItemGroup();
		//group->setFiltersChildEvents(false);
		group->setHandlesChildEvents(false);
		setGraphicsItem(group);
	}
	
	addHandle(Top | Left);
	addHandle(Top | Right);
	addHandle(Bottom | Left);
	addHandle(Bottom | Right);
	addHandle(Top);
	addHandle(Bottom);
	addHandle(Left);
	addHandle(Right);
	
	graphicsItem()->setVisible(false);
	
	connect(layerScene(), SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(onCurrentChanged(LayerRef)));
	connect(layerScene(), SIGNAL(layerPropertyChanged(LayerRef)), this, SLOT(onLayerPropertyChanged(LayerRef)));
	connect(canvas->viewController(), SIGNAL(transformUpdated()), this, SLOT(moveHandles()));
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

void RectTool::tabletPressEvent(CanvasTabletEvent *event)
{
	if (graphicsItem()->scene()->itemAt(event->viewPos))
	{
		event->ignore();
		return;
	}
	
	auto layer = layerScene()->rootLayer()->descendantAt(event->data.pos.toQPoint());
	
	if (layer)
		layerScene()->setCurrent(layer);
	
	// inserting new rect
	if (!d->rectLayer || !layer)
	{
		d->inserter.reset(new RectInserter(this));
		d->inserter->press(event->data);
	}
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	if (d->inserter)
	{
		d->inserter->move(event->data);
	}
}

void RectTool::tabletReleaseEvent(CanvasTabletEvent *event)
{
	Q_UNUSED(event)
	
	if (d->inserter)
	{
		d->inserter->release();
		d->inserter.reset();
	}
}

void RectTool::onCurrentChanged(const LayerRef &layer)
{
	clearLayerDelegation();
	
	d->current = layer;
	
	if (layer)
	{
		auto rectLayer = dynamic_cast<const RectLayer *>(layer.pointer());
		if (rectLayer)
		{
			d->rectLayer.reset(static_cast<RectLayer *>(rectLayer->clone()));
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
	if (d->current == layer && d->rectLayer && layer->isType<RectLayer>())
	{
		d->rectLayer.reset(static_cast<RectLayer *>(layer->clone()));
	}
}

void RectTool::addHandle(int handleTypes)
{
	auto handle = new RectHandleItem(this, handleTypes, graphicsItem());
	handle->setVisible(true);
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
	
	QPointSet oldKeys = d->rectLayer->tileKeys();
	
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
	emit requestUpdate(d->rectLayer->tileKeys() | oldKeys);
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
