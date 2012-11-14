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

struct ToolDeclaration
{
	ToolDeclaration() {}
	ToolDeclaration(const QString &text, const QIcon &icon, const QVector<int> &supportedLayerTypes) : text(text), icon(icon), supportedLayerTypes(supportedLayerTypes) {}
	QString text;
	QIcon icon;
	QVector<int> supportedLayerTypes;
};
typedef QHash<QString, ToolDeclaration> ToolDeclarationHash;

struct ActionDeclaration
{
	ActionDeclaration() {}
	ActionDeclaration(const QString &text, const QKeySequence &defaultShortcut = QKeySequence()) : text(text), shortcut(defaultShortcut) {}
	QString text;
	QKeySequence shortcut;
};
typedef QHash<QString, ActionDeclaration> ActionDeclarationHash;

struct SidebarDeclaration
{
	SidebarDeclaration() {}
	SidebarDeclaration(const QString &text) : text(text) {}
	QString text;
};
typedef QHash<QString, SidebarDeclaration> SideBarDeclarationHash;

struct ToolbarDeclaration
{
	ToolbarDeclaration() {}
	ToolbarDeclaration(const QString &text) : text(text) {}
	QString text;
};
typedef QHash<QString, ToolbarDeclaration> ToolBarDeclarationHash;

struct MenuDeclaration
{
	MenuDeclaration() {}
	MenuDeclaration(const QString &text) : text(text) {}
	QString text;
};

typedef QHash<QString, MenuDeclaration> MenuDeclarationHash;

}

#endif // FSGLOBAL_H
