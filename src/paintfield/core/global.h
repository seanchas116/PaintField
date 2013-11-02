#pragma once

#include <Qt>
#include <QEvent>
#include <QDebug>
#include <amulet/range_extension.hh>
#include <amulet/type_traits.hh>
#include <memory>

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

/**
 * Alias for std::shared_ptr
 */
template <class T>
using SP = std::shared_ptr<T>;

/**
 * Alias for std::weak_ptr
 */
template <class T>
using WP = std::weak_ptr<T>;

/**
 * Alias for std::make_shared
 */
template <class T, class... Args>
inline SP<T> makeSP(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

/**
 * Alias for std::static_pointer_cast
 */
template <class T, class U>
inline SP<T> staticSPCast(const SP<U> &r)
{
	return std::static_pointer_cast<T>(r);
}

/**
 * Alias for std::dynamic_pointer_cast
 */
template <class T, class U>
inline SP<T> dynamicSPCast(const SP<U> &r)
{
	return std::dynamic_pointer_cast<T>(r);
}

/**
 * Alias for std::const_pointer_cast.
 */
template <class T, class U>
inline SP<T> constSPCast(const SP<U> &r)
{
	return std::const_pointer_cast<T>(r);
}

/**
 * Postfix increment operator for ranges (containers) to wrap existing ranges in Amulet::RangeExtension wrapper.
 * Calls Amulet::extend. After this operator, you can use Amulet::RangeExtension methods for ranges.
 * (such as filter, map, unique...)
 */
template <class TRange, class = typename std::enable_if<Amulet::IsRange<TRange>::value>::type>
inline auto operator++(const TRange &range, int) -> decltype(Amulet::extend(range))
{
	return Amulet::extend(range);
}


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
	RoleStrokeEnabled,
	
	RoleRect,
	RoleRectShapeType,
	
	RoleText,
	RoleFont,
	RoleTextAlignment
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

/**
 * qWarning() with function info (Q_FUNC_INFO)
 */
#define PAINTFIELD_WARNING qWarning() << Q_FUNC_INFO << ":"

/**
 * qDebug() with function info (Q_FUNC_INFO)
 */
#define PAINTFIELD_DEBUG qDebug() << Q_FUNC_INFO << ":"


}
