#ifndef FSGLOBAL_H
#define FSGLOBAL_H

#include <Qt>
#include <QEvent>
#include <QMetaType>
#include <QHash>
#include <QString>
#include <QIcon>
#include <QKeySequence>

namespace PaintField
{

enum LayerPropertyRole
{
	RoleName = Qt::UserRole,
	RoleType,
	RoleVisible,
	RoleLocked,
	RoleOpacity,
	RoleBlendMode,
	RoleThumbnail,
	RoleSurface
};

enum EventType
{
	EventCanvasTabletMove = QEvent::User,
	EventCanvasTabletPress,
	EventCanvasTabletRelease,
	
	EventWidgetTabletMove,
	EventWidgetTabletPress,
	EventWidgetTabletRelease,
	
	EventCanvasMouseMove,
	EventCanvasMousePress,
	EventCanvasMouseRelease,
	EventCanvasMouseDoubleClick
};

enum ThumbnailSize
{
	ThumbnailSize = 48
};

enum EditActionType
{
	EditActionCopy,
	EditActionCut,
	EditActionPaste,
	EditActionDelete,
	EditActionSelectAll
};

}

#endif // FSGLOBAL_H
