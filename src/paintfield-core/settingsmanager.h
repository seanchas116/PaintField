#ifndef PAINTFIELD_SETTINGSMANAGER_H
#define PAINTFIELD_SETTINGSMANAGER_H

#include <QObject>
#include <QIcon>
#include <QHash>
#include <QKeySequence>

namespace PaintField {

struct ToolDeclaration
{
	ToolDeclaration() {}
	ToolDeclaration(const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes) : text(text), icon(icon), supportedLayerTypes(supportedLayerTypes) {}
	QString text;
	QIcon icon;
	QStringList supportedLayerTypes;
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



class SettingsManager : public QObject
{
	Q_OBJECT
public:
	explicit SettingsManager(QObject *parent = 0);
	~SettingsManager();
	
	void loadBuiltinSettings();
	void loadUserSettings();
	
	void loadMenuBarOrderFromJson(const QString &path);
	void loadWorkspaceItemOrderFromJson(const QString &path);
	void loadAndAddKeyBindingsFromJson(const QString &path);
	
	void setMenuBarOrder(const QVariant &order);
	QVariant menuBarOrder() const;
	
	void setWorkspaceItemOrder(const QVariant &order);
	QVariant workspaceItemOrder() const;
	
	void declareTool(const QString &name, const ToolDeclaration &info);
	void declareAction(const QString &name, const ActionDeclaration &info);
	void declareSideBar(const QString &name, const SidebarDeclaration &info);
	void declareToolbar(const QString &name, const ToolbarDeclaration &info);
	void declareMenu(const QString &id, const MenuDeclaration &info);
	
	void declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes);
	void declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut = QKeySequence());
	
	ToolDeclarationHash toolDeclarationHash() const;
	ActionDeclarationHash actionDeclarationHash() const;
	SideBarDeclarationHash sideBarDeclarationHash() const;
	ToolBarDeclarationHash toolBarDeclarationHash() const;
	MenuDeclarationHash menuDeclarationHash() const;
	
	QStringList toolNames() const;
	QStringList actionNames() const;
	QStringList sidebarNames() const;
	QStringList toolbarNames() const;
	QStringList menuNames() const;
	
	QHash<QString, QKeySequence> keyBindingHash() const;
	void addKeyBindingHash(const QHash<QString, QKeySequence> &hash);
	void addKeyBinding(const QString &name, const QKeySequence &shortcut);
	
	QString builtinDataDir() const;
	QString userDataDir() const;
	
	QString lastFileDialogPath() const;
	void setLastFileDialogPath(const QString &path);
	
signals:
	
public slots:
	
private:
	
	void applyKeyBindingsToActionDeclarations();
	
	class Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_SETTINGSMANAGER_H
