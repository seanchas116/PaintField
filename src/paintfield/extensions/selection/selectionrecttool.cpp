#include "selectionrecttool.h"
#include "paintfield/core/selection.h"

namespace PaintField {

struct SelectionRectTool::Data
{
	SelectionRectTool *mSelf;
	Type mType;
	Malachite::Vec2D mStart;
	SelectionSurface mOldSurface;
	QPointSet mOldKeys;

	void updateRect(const QRect &rect) {

		auto surface = mOldSurface;
		QPainterPath path;
		switch (mType) {
			case TypeRect:
				path.addRect(rect);
				break;
			case TypeEllipse:
				path.addEllipse(rect);
				break;
			default:
				break;
		}
		auto keys = SelectionDrawUtil::drawPath(surface, path);
		mSelf->document()->selection()->updateSurface(surface, keys | mOldKeys);
		mOldKeys = keys;
	}

	static QRect makeRect(const QPointF &p1, const QPointF &p2)
	{
		auto leftRight = std::minmax(p1.x(), p2.x());
		auto topBottom = std::minmax(p1.y(), p2.y());
		return QRectF(QPointF(leftRight.first, topBottom.first), QPointF(leftRight.second, topBottom.second)).toAlignedRect();
	}
};

SelectionRectTool::SelectionRectTool(Type type, Canvas *parent) :
	Tool(parent),
	d(new Data)
{
	d->mSelf = this;
	d->mType = type;
}

SelectionRectTool::~SelectionRectTool()
{
}

int SelectionRectTool::cursorPressEvent(CanvasCursorEvent *event)
{
	d->mStart = event->data.pos;
	d->mOldSurface = document()->selection()->surface();
	return 0;
}

void SelectionRectTool::cursorMoveEvent(CanvasCursorEvent *event, int type)
{
	if (type == 0) {
		d->updateRect(Data::makeRect(d->mStart, event->data.pos));
	}
}

void SelectionRectTool::cursorReleaseEvent(CanvasCursorEvent *event, int type)
{
	Q_UNUSED(event);
	if (type == 0) {
		document()->selection()->commitUpdates();
	}
}


} // namespace PaintField
