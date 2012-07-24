#ifndef FSGLOBAL_H
#define FSGLOBAL_H

#include <Qt>
#include <QEvent>
#include <QMetaType>

namespace FSGlobal {

enum LayerPropertyRole {
	RoleName = Qt::UserRole,
	RoleType,
	RoleVisible,
	RoleLocked,
	RoleOpacity,
	RoleBlendMode,
	RoleThumbnail,
	RoleSurface
};

enum Event {
	EventTabletMove = QEvent::User,
	EventTabletPress,
	EventTabletRelease
};

enum ImageDataFormat {
	
};

enum ThumbnailSize {
	ThumbnailSize = 48
};

enum EditAction {
	EditActionCopy,
	EditActionCut,
	EditActionPaste,
	EditActionDelete,
	EditActionSelectAll
};

}

#endif // FSGLOBAL_H
