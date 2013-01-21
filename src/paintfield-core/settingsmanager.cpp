#include <QtCore>
#include <QDesktopServices>
#include "util.h"

#include "settingsmanager.h"

namespace PaintField {

struct SettingsManager::Data
{
	QVariant menuBarOrder, workspaceItemOrder;
	
	ToolDeclarationHash toolDeclarationHash;
	ActionDeclarationHash actionDeclarationHash;
	SideBarDeclarationHash sideBarDeclarationHash;
	ToolBarDeclarationHash toolbarInfoHash;
	MenuDeclarationHash menuDeclarationHash;
	
	QHash<QString, QKeySequence> keyBindingHash;
	
	QString lastFileDialogPath;
};

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	// prepare directiries
	
	//QDir documentDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
	//documentDir.mkpath("PaintField/Contents/Brush Presets");
}

SettingsManager::~SettingsManager()
{
	delete d;
}

static const QString menuBarOrderFileName("menubar.json");
static const QString WorkspaceItemOrderFileName("workspace-items.json");
static const QString keyBindingFileName("key-bindings.json");

void SettingsManager::loadBuiltinSettings()
{
	QDir dir(builtinDataDir());
	
	if (!dir.exists())
		return;
	
	if (!dir.cd("Settings"))
		return;
	
	loadMenuBarOrderFromJson(dir.filePath(menuBarOrderFileName));
	loadWorkspaceItemOrderFromJson(dir.filePath(WorkspaceItemOrderFileName));
	loadAndAddKeyBindingsFromJson(dir.filePath(keyBindingFileName));
	
	applyKeyBindingsToActionDeclarations();
}

void SettingsManager::loadUserSettings()
{
	QDir dir(userDataDir());
	
	if (!dir.exists())
		return;
	
	if (!dir.cd("Settings"))
		return;
	
	QString menubarPath = dir.filePath(menuBarOrderFileName);
	QString workspaceItemPath = dir.filePath(WorkspaceItemOrderFileName);
	QString keymapPath = dir.filePath(keyBindingFileName);
	
	if (QFile::exists(menubarPath))
		loadMenuBarOrderFromJson(menubarPath);
	
	if (QFile::exists(workspaceItemPath))
		loadWorkspaceItemOrderFromJson(workspaceItemPath);
	
	if (QFile::exists(keymapPath))
		loadAndAddKeyBindingsFromJson(keymapPath);
	
	applyKeyBindingsToActionDeclarations();
}

void SettingsManager::loadMenuBarOrderFromJson(const QString &path)
{
	d->menuBarOrder = loadJsonFromFile(path);
}

void SettingsManager::loadWorkspaceItemOrderFromJson(const QString &path)\
{
	d->workspaceItemOrder = loadJsonFromFile(path);
}

void SettingsManager::loadAndAddKeyBindingsFromJson(const QString &path)
{
	QVariantMap map = loadJsonFromFile(path).toMap();
	
	for (auto iter = map.begin(); iter != map.end(); ++iter)
	{
		QString id = iter.key();
		QKeySequence key(iter.value().toString());
		
		if (!id.isEmpty() && !key.isEmpty())
			addKeyBinding(id, key);
	}
}

void SettingsManager::setMenuBarOrder(const QVariant &order)
{
	d->menuBarOrder = order;
}

QVariant SettingsManager::menuBarOrder() const { return d->menuBarOrder; }

void SettingsManager::setWorkspaceItemOrder(const QVariant &order)
{
	d->workspaceItemOrder = order;
}

QVariant SettingsManager::workspaceItemOrder() const { return d->workspaceItemOrder; }

void SettingsManager::declareTool(const QString &name, const ToolDeclaration &info)
{
	d->toolDeclarationHash[name] = info;
}

void SettingsManager::declareAction(const QString &name, const ActionDeclaration &info)
{
	d->actionDeclarationHash[name] = info;
}

void SettingsManager::declareSideBar(const QString &name, const SidebarDeclaration &info)
{
	d->sideBarDeclarationHash[name] = info;

}

void SettingsManager::declareToolbar(const QString &name, const ToolbarDeclaration &info)
{
	d->toolbarInfoHash[name] = info;
}

void SettingsManager::declareMenu(const QString &id, const MenuDeclaration &info)
{
	d->menuDeclarationHash[id] = info;
}

void SettingsManager::declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes)
{
	declareTool(name, ToolDeclaration(text, icon, supportedLayerTypes));
}

void SettingsManager::declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut)
{
	declareAction(name, ActionDeclaration(text, defaultShortcut));
}

ToolDeclarationHash SettingsManager::toolDeclarationHash() const { return d->toolDeclarationHash; }
ActionDeclarationHash SettingsManager::actionDeclarationHash() const { return d->actionDeclarationHash; }
SideBarDeclarationHash SettingsManager::sideBarDeclarationHash() const { return d->sideBarDeclarationHash; }
ToolBarDeclarationHash SettingsManager::toolBarDeclarationHash() const { return d->toolbarInfoHash; }
MenuDeclarationHash SettingsManager::menuDeclarationHash() const { return d->menuDeclarationHash; }

QStringList SettingsManager::toolNames() const { return d->toolDeclarationHash.keys(); }
QStringList SettingsManager::actionNames() const { return d->actionDeclarationHash.keys(); }
QStringList SettingsManager::sidebarNames() const { return d->sideBarDeclarationHash.keys(); }
QStringList SettingsManager::toolbarNames() const { return d->toolbarInfoHash.keys(); }
QStringList SettingsManager::menuNames() const { return d->menuDeclarationHash.keys(); }

QHash<QString, QKeySequence> SettingsManager::keyBindingHash() const { return d->keyBindingHash; }

void SettingsManager::addKeyBindingHash(const QHash<QString, QKeySequence> &hash)
{
	for (auto iter = hash.begin(); iter != hash.end(); ++iter)
		addKeyBinding(iter.key(), iter.value());
}

void SettingsManager::addKeyBinding(const QString &name, const QKeySequence &shortcut)
{
	d->keyBindingHash[name] = shortcut;
}

void SettingsManager::applyKeyBindingsToActionDeclarations()
{
	for (auto keyIter = d->keyBindingHash.begin(); keyIter != d->keyBindingHash.end(); ++keyIter)
	{
		for (auto actionIter = d->actionDeclarationHash.begin(); actionIter != d->actionDeclarationHash.end(); ++actionIter)
		{
			if (actionIter.key() == keyIter.key())
				actionIter->shortcut = keyIter.value();
		}
	}
}

QString SettingsManager::builtinDataDir() const
{
	return QDir(qApp->applicationDirPath()).absolutePath();
}

QString SettingsManager::userDataDir() const
{
	return QDir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).filePath("PaintField");
}

QString SettingsManager::lastFileDialogPath() const { return d->lastFileDialogPath; }
void SettingsManager::setLastFileDialogPath(const QString &path) { d->lastFileDialogPath = path; }

} // namespace PaintField
