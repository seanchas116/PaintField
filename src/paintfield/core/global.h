#pragma once

#include <Qt>
#include <QEvent>
#include <QDebug>

#if defined(PAINTFIELD_LIBRARY)
#define PAINTFIELDCORE_EXPORT Q_DECL_EXPORT
#else
#define PAINTFIELDCORE_EXPORT Q_DECL_IMPORT
#endif

#if defined(Q_OS_MAC) && !defined(PF_FORCE_RASTER_ENGINE)
#define PAINTFIELD_COREGRAPHICS_REPAINT
#endif

namespace PaintField
{

enum LayerPropertyRole
{
	RoleName = Qt::UserRole,
	RoleVisible,
	RoleLocked,
	RoleOpacity,
	RoleBlendMode,
	RoleThumbnail,
	
	RoleSurface,
	
	RoleStrokePosition,
	RoleStrokeWidth,
	RoleJoinStyle,
	RoleCapStyle,
	RoleFillBrush,
	RoleStrokeBrush,
	RoleFillEnabled,
	RoleStrokeEnabled
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

enum StrokePosition
{
	StrokePositionInside,
	StrokePositionCenter,
	StrokePositionOutside
};

#define PAINTFIELD_WARNING qWarning() << Q_FUNC_INFO << ":"
#define PAINTFIELD_DEBUG qDebug() << Q_FUNC_INFO << ":"

}
