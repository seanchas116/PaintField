#include <QApplication>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QAction>
#include <QItemSelectionModel>

#include <functional>
#include <boost/signals2.hpp>

#include "paintfield/core/rasterlayer.h"
#include "paintfield/extensions/layerui/layeruicontroller.h"
#include "paintfield/core/canvas.h"
#include "paintfield/core/textlayer.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/rectlayer.h"
#include "paintfield/core/layeritemmodel.h"
#include "paintfield/core/layeredit.h"
#include "paintfield/core/canvasupdatemanager.h"

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
	
	void setPath(const QPainterPath &path) { m_path = path; }
	QPainterPath path() const { return m_path; }
	
	QRectF boundingRect() const override { return m_path.boundingRect(); }
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
	{
		Q_UNUSED(option);
		Q_UNUSED(widget);
		
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
	
	RectHandleItem(int handleFlags, QGraphicsItem *parent = 0) :
		QGraphicsItem(parent),
		m_handleFlags(handleFlags)
	{}
	
	QRectF boundingRect() const override
	{
		return QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
	{
		Q_UNUSED(option);
		Q_UNUSED(widget);
		
		QRect rect(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
		QRect innerRect = rect.adjusted(1,1,-1,-1);
		
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::black);
		painter->drawRect(rect);
		painter->setBrush(Qt::white);
		painter->drawRect(innerRect);
	}
	
	void setOnHandleMoved(const std::function<void (const QPointF &, int)> &f) { m_onHandleMoved = f; }
	void setOnHandleMoveFinished(const std::function<void ()> &f) { m_onHandleMoveFinished = f; }
	
	int handleFlags() const { return m_handleFlags; }
	
	void invertHandleFlagsLeftRight()
	{
		if (m_handleFlags & Left)
		{
			m_handleFlags &= ~Left;
			m_handleFlags |= Right;
		}
		else if (m_handleFlags & Right)
		{
			m_handleFlags &= ~Right;
			m_handleFlags |= Left;
		}
	}
	
	void invertHandleFlagsTopBottom()
	{
		if (m_handleFlags & Top)
		{
			m_handleFlags &= ~Top;
			m_handleFlags |= Bottom;
		}
		else if (m_handleFlags & Bottom)
		{
			m_handleFlags &= ~Bottom;
			m_handleFlags |= Top;
		}
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
		m_onHandleMoved(newPos, m_handleFlags);
	}
	
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{
		Q_UNUSED(event);
		m_onHandleMoveFinished();
	}
	
private:
	
	QPointF m_dragStartPos;
	QPointF m_originalPos;
	int m_handleFlags;
	int m_radius = handleRadius;
	std::function<void (const QPointF &, int)> m_onHandleMoved;
	std::function<void ()> m_onHandleMoveFinished;
};

struct RectTool::Data
{
	LayerUIController *layerController = 0;
	
	// graphics items
	QList<RectHandleItem *> handles;
	FrameGraphicsItem *frameItem = 0;
	
	RectHandleItem *findHandle(int types)
	{
		for (auto handle : handles)
		{
			if (handle->handleFlags() == types)
				return handle;
		}
		return 0;
	}
	
	struct LayerInfo
	{
		LayerConstPtr original; // original layer
		
		std::shared_ptr<AbstractRectLayer> rectLayer; // editable rect layer (only if original layer is rect)
		Vec2D originalRectPos;
		
		QRect rasterBoundingRect;
		QPoint rasterOffset;
		
		void setOriginalLayer(const LayerConstPtr &layer)
		{
			original = layer;
			
			auto originalRectLayer = std::dynamic_pointer_cast<const AbstractRectLayer>(layer);
			if (originalRectLayer)
			{
				rectLayer = std::static_pointer_cast<AbstractRectLayer>(layer->clone());
				originalRectPos = rectLayer->rect().topLeft();
			}
			
			auto rasterLayer = std::dynamic_pointer_cast<const RasterLayer>(layer);
			if (rasterLayer)
			{
				rasterBoundingRect = rasterLayer->surface().boundingRect();
				rasterOffset = QPoint();
			}
		}
	};
	
	QList<LayerInfo> selectedLayerInfos;
	
	Mode mode = NoOperation;
	bool dragDistanceEnough = false;
	Vec2D dragStartPos;
	Vec2D originalRectPos;
	
	QPointSet rectKeysWithHandleMargin(const QRectF &rect) const
	{
		auto newRect = rect.adjusted(-handleRadius, -handleRadius, handleRadius, handleRadius);
		return Surface::rectToKeys(newRect.toAlignedRect());
	}
	
	AddingType addingType = NoAdding;
	std::shared_ptr<AbstractRectLayer> layerToAdd;
	LayerConstPtr layerToAddParent;
	int layerToAddIndex;
	
	LayerConstPtr clickedLayer;
	bool clickedWithShift = false;
	
	SelectingMode selectingMode = SelectImmediately;
	
	CanvasUpdateManager *updateManager = 0;
};

RectTool::RectTool(AddingType type, Canvas *canvas) :
	Tool(canvas),
	d(new Data)
{
	d->layerController = canvas->findChild<LayerUIController *>();
	d->updateManager = new CanvasUpdateManager(this);
	connect(d->updateManager, SIGNAL(updateTilesRequested(QPointSet)), this, SIGNAL(requestUpdate(QPointSet)));
	
	// set modes
	
	d->addingType = type;
	
	if (d->addingType == NoAdding)
		d->selectingMode = SelectImmediately;
	else
		d->selectingMode = SelectLater;
	
	// create graphics items
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
	
	connect(layerScene(), SIGNAL(selectionChanged(QList<LayerConstPtr>,QList<LayerConstPtr>)), this, SLOT(updateSelected()));
	connect(layerScene(), SIGNAL(layerChanged(LayerConstPtr)), this, SLOT(updateLayer(LayerConstPtr)));
	connect(canvas, SIGNAL(transformsChanged(std::shared_ptr<const CanvasTransforms>)), this, SLOT(updateGraphicsItems()));
	updateSelected();
}

RectTool::~RectTool()
{
	delete d;
}

void RectTool::drawLayer(SurfacePainter *painter, const LayerConstPtr &layer)
{
	for (const auto &info : d->selectedLayerInfos)
	{
		if (info.original == layer)
		{
			if (info.rectLayer)
			{
				info.rectLayer->render(painter);
			}
			else
			{
				auto rasterLayer = std::dynamic_pointer_cast<const RasterLayer>(layer);
				if (rasterLayer)
				{
					painter->drawSurface(info.rasterOffset, rasterLayer->surface());
				}
			}
		}
	}
}

void RectTool::setSelectingMode(SelectingMode mode)
{
	d->selectingMode = mode;
}

RectTool::SelectingMode RectTool::selectingMode() const
{
	return d->selectingMode;
}

RectTool::AddingType RectTool::addingType() const
{
	return d->addingType;
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
	
	d->updateManager->setEnabled(true);
	
	d->clickedLayer = layerScene()->rootLayer()->descendantAt(event->data.pos.toQPoint(), handleRadius);
	d->clickedWithShift = event->modifiers() & Qt::ShiftModifier;
	
	if (d->selectingMode == SelectImmediately)
	{
		// select immediately the clicked layer
		selectLayer(d->clickedLayer, d->clickedWithShift);
	}
	else
	{
		// the clicked layer is already selected, selection is unchanged and current is set to the clicked layer
		if (layerScene()->selection().contains(d->clickedLayer))
		{
			layerScene()->setCurrent(d->clickedLayer);
		}
		// clear selection to insert a new layer
		else
		{
			layerScene()->setSelection({});
		}
	}
	
	// decide mode
	
	// if already layers are selected and a layer is clicked, mode will be Dragging
	if (d->selectedLayerInfos.size() && d->clickedLayer)
		d->mode = Dragging;
	// if no layer is selected or no layer is clicked, go to Inserting mode if addingType is not NoAdding
	else if (d->addingType != NoAdding)
		d->mode = Inserting;
	
	// there is nothing to do
	if (d->mode == NoOperation)
		return;
	
	// save drag origin
	d->dragDistanceEnough = false;
	d->dragStartPos = event->data.pos;
}

void RectTool::tabletMoveEvent(CanvasTabletEvent *event)
{
	auto delta = event->data.pos - d->dragStartPos;
	
	if ( d->dragDistanceEnough )
	{
		switch (d->mode)
		{
			default:
				break;
				
			case Dragging:
			case Inserting:
			{
				QPointSet keys;
				
				if (d->mode == Dragging)
				{
					QRectF wholeRect;
					
					for (auto &info : d->selectedLayerInfos)
					{
						// rect layer
						if (info.rectLayer)
						{
							auto rect = info.rectLayer->rect();
							wholeRect |= rect;
							rect.moveTopLeft(delta + info.originalRectPos);
							wholeRect |= rect;
							info.rectLayer->setRect(rect);
						}
						// raster layer
						else if (info.original->isType<RasterLayer>())
						{
							wholeRect |= info.rasterBoundingRect.translated(info.rasterOffset);
							info.rasterOffset = delta.toQPoint();
							wholeRect |= info.rasterBoundingRect.translated(info.rasterOffset);
						}
					}
					
					keys = d->rectKeysWithHandleMargin(wholeRect);
				}
				else // inserting
				{
					QRectF rect = d->layerToAdd->rect();
					
					keys |= d->rectKeysWithHandleMargin(rect);
					
					auto p1 = d->dragStartPos, p2 = event->data.pos;
					rect.setLeft(std::min(p1.x(), p2.x()));
					rect.setRight(std::max(p1.x(), p2.x()));
					rect.setTop(std::min(p1.y(), p2.y()));
					rect.setBottom(std::max(p1.y(), p2.y()));
					
					keys |= d->rectKeysWithHandleMargin(rect);
					
					d->layerToAdd->setRect(rect);
				}
				
				d->updateManager->addTiles(keys);
				//emit requestUpdate(keys);
				updateGraphicsItems();
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
	
	d->updateManager->setEnabled(false);
	
	bool selectLater = false;
	
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
	else if (d->mode == Inserting && d->selectingMode == SelectLater)
	{
		selectLater = true;
	}
	
	d->mode = NoOperation;
	updateGraphicsItems();
	
	if (selectLater)
	{
		selectLayer(d->clickedLayer, d->clickedWithShift);
	}
}

void RectTool::updateSelected()
{
	auto layers = layerScene()->selection();
	
	clearLayerDelegation();
	
	d->selectedLayerInfos.clear();
	
	for (const auto &layer : layers)
	{
		Data::LayerInfo info;
		info.setOriginalLayer(layer);
		
		if (layer->isType<AbstractRectLayer>() || layer->isType<RasterLayer>())
			addLayerDelegation(layer);
		
		d->selectedLayerInfos << info;
	}
	
	updateGraphicsItems();
}

void RectTool::updateLayer(const LayerConstPtr &layer)
{
	for (auto &info : d->selectedLayerInfos)
	{
		if (info.original == layer)
			info.setOriginalLayer(layer);
	}
	updateGraphicsItems();
}

void RectTool::addHandle(int handleTypes, qreal zValue)
{
	auto handle = new RectHandleItem(handleTypes, graphicsItem());
	
	handle->setVisible(true);
	handle->setZValue(zValue);
	d->handles << handle;
	
	handle->setOnHandleMoved(std::bind(&RectTool::onHandleMoved, this, std::placeholders::_1, std::placeholders::_2));
	handle->setOnHandleMoveFinished(std::bind(&RectTool::onHandleMoveFinished, this));
}

void RectTool::updateGraphicsItems()
{
	updateHandles();
	updateFrameRect();
}

void RectTool::updateHandles()
{
	if (d->selectedLayerInfos.size() == 1 && d->mode == NoOperation)
	{
		auto rectLayer = d->selectedLayerInfos.at(0).rectLayer;
		if (rectLayer)
		{
			for (auto handle : d->handles)
				handle->setVisible(true);
			
			auto rect = rectLayer->rect();
			
			// get vertices in scene coordinates
			auto transformToView = canvas()->transforms()->sceneToView;
			auto topLeft = rect.topLeft() * transformToView;
			auto topRight = rect.topRight() * transformToView;
			auto bottomLeft = rect.bottomLeft() * transformToView;
			auto bottomRight = rect.bottomRight() * transformToView;
			
			d->findHandle(Left)->setPos( (topLeft + bottomLeft) * 0.5 );
			d->findHandle(Right)->setPos( (topRight + bottomRight) * 0.5 );
			d->findHandle(Top)->setPos( (topLeft + topRight) * 0.5 );
			d->findHandle(Bottom)->setPos( (bottomLeft + bottomRight) * 0.5 );
			
			d->findHandle(Left | Top)->setPos(topLeft);
			d->findHandle(Left | Bottom)->setPos(bottomLeft);
			d->findHandle(Right | Top)->setPos(topRight);
			d->findHandle(Right | Bottom)->setPos(bottomRight);
			
			return;
		}
	}
	
	for (auto handle : d->handles)
		handle->setVisible(false);
}

void RectTool::updateFrameRect()
{
	QRectF rect;
	
	if (d->mode == Inserting)
	{
		d->frameItem->setVisible(true);
		rect = d->layerToAdd->rect();
	}
	else
	{
		if (!(d->selectedLayerInfos.size() == 1 && d->selectedLayerInfos.at(0).rectLayer->isType<RectLayer>()))
		{
			for (const auto &info : d->selectedLayerInfos)
			{
				if (info.rectLayer)
					rect |= info.rectLayer->rect();
				else if (info.original->isType<RasterLayer>())
					rect |= info.rasterBoundingRect.translated(info.rasterOffset);
			}
		}
	}
	
	// set path
	if (rect.isValid())
	{
		QPainterPath path;
		path.addRect(rect);
		
		d->frameItem->setVisible(true);
		d->frameItem->setPath(path * canvas()->transforms()->sceneToView);
	}
	else
		d->frameItem->setVisible(false);
}

void RectTool::onHandleMoved(const QPointF &pos, int handleFlags)
{
	if (d->selectedLayerInfos.size() != 1)
		return;
	
	auto rectLayer = d->selectedLayerInfos.at(0).rectLayer;
	
	if (!rectLayer)
		return;
	
	QPointSet keys;
	
	auto rect = rectLayer->rect();
	keys |= d->rectKeysWithHandleMargin(rect);
	
	auto scenePos = pos * canvas()->transforms()->viewToScene;
	
	double left = rect.left();
	double right = rect.right();
	double top = rect.top();
	double bottom = rect.bottom();
	
	if (handleFlags & Left)
		left = scenePos.x();
	
	if (handleFlags & Right)
		right = scenePos.x();
	
	if (handleFlags & Top)
		top = scenePos.y();
	
	if (handleFlags & Bottom)
		bottom = scenePos.y();
	
	if (right < left)
	{
		std::swap(left, right);
		
		for (RectHandleItem *handle : d->handles)
			handle->invertHandleFlagsLeftRight();
	}
	
	if (bottom < top)
	{
		std::swap(top, bottom);
		
		for (RectHandleItem *handle : d->handles)
			handle->invertHandleFlagsTopBottom();
	}
	
	rect.setCoords(left, top, right, bottom);
	keys |= d->rectKeysWithHandleMargin(rect);
	
	rectLayer->setRect(rect);
	
	emit requestUpdate(keys);
	
	updateGraphicsItems();
}

void RectTool::onHandleMoveFinished()
{
	updateGraphicsItems();
	commit();
}

void RectTool::commit()
{
	// setting property 
	for (const auto &info : d->selectedLayerInfos)
	{
		if (info.rectLayer)
			layerScene()->setLayerProperty(info.original, info.rectLayer->rect(), RoleRect, tr("Change Rect"));
		else if (info.original->isType<RasterLayer>())
			layerScene()->editLayer(info.original, new LayerMoveEdit(info.rasterOffset), tr("Move Layer"));
	}
}

void RectTool::startAdding()
{
	auto createNew = [this]()
	{
		std::shared_ptr<AbstractRectLayer> layer;
		
		if (d->addingType == RectTool::AddText)
		{
			auto textLayer = std::make_shared<TextLayer>();
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
			layer = std::make_shared<RectLayer>();
			
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
	
	d->layerToAdd = createNew();
	
	auto current = layerScene()->current();
	if (current)
	{
		d->layerToAddParent = current->parent();
		d->layerToAddIndex = current->index();
	}
	else
	{
		d->layerToAddParent = layerScene()->rootLayer();
		d->layerToAddIndex = 0;
	}
	
	addLayerInsertion(d->layerToAddParent, d->layerToAddIndex, d->layerToAdd);
}

void RectTool::finishAdding()
{
	PAINTFIELD_DEBUG;
	
	clearLayerInsertions();
	layerScene()->addLayers({d->layerToAdd}, d->layerToAddParent, d->layerToAddIndex, tr("Add Rectangle"));
	layerScene()->setCurrent(d->layerToAdd);
	layerScene()->setSelection({d->layerToAdd});
	d->layerToAdd = nullptr;
}

void RectTool::selectLayer(const LayerConstPtr &layer, bool isShiftPressed)
{
	auto selection = layerScene()->selection();
	
	if (!selection.contains(layer))
	{
		if (isShiftPressed)
		{
			selection << layer;
			layerScene()->setSelection(selection);
		}
		else
		{
			layerScene()->setSelection({layer});
		}
	}
	
	layerScene()->setCurrent(layer);
}

} // namespace PaintField
