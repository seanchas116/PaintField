#ifndef PAINTFIELD_SETTINGSMANAGER_H
#define PAINTFIELD_SETTINGSMANAGER_H

#include <QObject>
#include <QIcon>
#include <QHash>
#include <QKeySequence>
#include <QVariant>

namespace PaintField {

struct ToolInfo
{
	ToolInfo() {}
	ToolInfo(const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes) : text(text), icon(icon), supportedLayerTypes(supportedLayerTypes) {}
	QString text;
	QIcon icon;
	QStringList supportedLayerTypes;
};

struct ActionInfo
{
	ActionInfo() {}
	ActionInfo(const QString &text, const QKeySequence &defaultShortcut = QKeySequence()) : text(text), shortcut(defaultShortcut) {}
	QString text;
	QKeySequence shortcut;
};

struct SideBarInfo
{
	SideBarInfo() {}
	SideBarInfo(const QString &text) : text(text) {}
	QString text;
};

struct ToolBarInfo
{
	ToolBarInfo() {}
	ToolBarInfo(const QString &text) : text(text) {}
	QString text;
};

struct MenuInfo
{
	MenuInfo() {}
	MenuInfo(const QString &text) : text(text) {}
	QString text;
};



class SettingsManager : public QObject
{
	Q_OBJECT
public:
	explicit SettingsManager(QObject *parent = 0);
	~SettingsManager();
	
	void loadBuiltinSettings();
	void loadUserSettings();
	
	void loadSettingsFromDir(const QString &dirPath);
	void loadSettingsFromJsonFile(const QString &path);
	
	QVariantMap &settings();
	void addSettings(const QVariantMap &settings);
	
	QVariant value(const QStringList &path, const QVariant &defaultValue = QVariant());
	
	void declareTool(const QString &name, const ToolInfo &info);
	void declareAction(const QString &name, const ActionInfo &info);
	void declareSideBar(const QString &name, const SideBarInfo &info);
	void declareToolbar(const QString &name, const ToolBarInfo &info);
	void declareMenu(const QString &id, const MenuInfo &info);
	
	void declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes);
	void declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut = QKeySequence());
	
	QHash<QString, ToolInfo> toolInfoHash() const;
	QHash<QString, ActionInfo> actionInfoHash() const;
	QHash<QString, SideBarInfo> sideBarInfoHash() const;
	QHash<QString, ToolBarInfo> toolBarInfoHash() const;
	QHash<QString, MenuInfo> menuInfoHash() const;
	
	QStringList toolNames() const;
	QStringList actionNames() const;
	QStringList sidebarNames() const;
	QStringList toolbarNames() const;
	QStringList menuNames() const;
	
	QString builtinDataDir() const;
	QString userDataDir() const;
	
	QString lastFileDialogPath() const;
	void setLastFileDialogPath(const QString &path);
	
signals:
	
public slots:
	
private:
	
	void applyKeyBindingsToActionDeclarations();
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_SETTINGSMANAGER_H
