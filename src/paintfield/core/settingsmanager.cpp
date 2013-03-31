#include <QtCore>
#include <QDesktopServices>
#include "util.h"

#include "settingsmanager.h"

namespace PaintField {

struct SettingsManager::Data
{
	QVariantMap settings;
	
	QVariant menuBarOrder, workspaceItemOrder;
	
	QHash<QString, ToolInfo> toolDeclarationHash;
	QHash<QString, ActionInfo> actionDeclarationHash;
	QHash<QString, SideBarInfo> sideBarDeclarationHash;
	QHash<QString, ToolBarInfo> toolbarInfoHash;
	QHash<QString, MenuInfo> menuDeclarationHash;
	
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
	
	loadSettingsFromDir(dir.path());
}

void SettingsManager::loadUserSettings()
{
	QDir dir(userDataDir());
	
	if (!dir.exists())
		return;
	
	if (!dir.cd("Settings"))
		return;
	
	loadSettingsFromDir(dir.path());
}

void SettingsManager::loadSettingsFromDir(const QString &dirPath)
{
	QDir dir(dirPath);
	
	if (!dir.exists())
		return;
	
	for (const QFileInfo &fileInfo : dir.entryInfoList(QDir::NoFilter, QDir::Name))
	{
		if (fileInfo.suffix() == "json")
			loadSettingsFromJsonFile(fileInfo.absoluteFilePath());
	}
	
	applyKeyBindingsToActionDeclarations();
}

void SettingsManager::loadSettingsFromJsonFile(const QString &path)
{
	addSettings(Util::loadJsonFromFile(path).toMap());
}

QVariantMap &SettingsManager::settings()
{
	return d->settings;
}

QVariant SettingsManager::value(const QStringList &path, const QVariant &defaultValue)
{
	auto map = d->settings;
	QVariant data;
	
	for (auto key : path)
	{
		if (!map.contains(key))
			return defaultValue;
		
		data = map[key];
		map = data.toMap();
	}
	
	return data;
}

QVariantMap joinSettings(const QVariantMap &dst, const QVariantMap &src)
{
	QVariantMap result = dst;
	
	for (auto iter = src.begin(); iter != src.end(); ++iter)
	{
		auto key = iter.key();
		auto value = iter.value();
		
		if (key.startsWith(".") && dst.contains(key)) // namespace
			result[key] = joinSettings(dst[key].toMap(), value.toMap());
		else
			result[key] = value;
	}
	
	return result;
}

void SettingsManager::addSettings(const QVariantMap &settings)
{
	d->settings = joinSettings(d->settings, settings);
}

void SettingsManager::declareTool(const QString &name, const ToolInfo &info)
{
	d->toolDeclarationHash[name] = info;
}

void SettingsManager::declareAction(const QString &name, const ActionInfo &info)
{
	d->actionDeclarationHash[name] = info;
}

void SettingsManager::declareSideBar(const QString &name, const SideBarInfo &info)
{
	d->sideBarDeclarationHash[name] = info;
}

void SettingsManager::declareToolbar(const QString &name, const ToolBarInfo &info)
{
	d->toolbarInfoHash[name] = info;
}

void SettingsManager::declareMenu(const QString &id, const MenuInfo &info)
{
	d->menuDeclarationHash[id] = info;
}

void SettingsManager::declareTool(const QString &name, const QString &text, const QIcon &icon, const QStringList &supportedLayerTypes)
{
	declareTool(name, ToolInfo(text, icon, supportedLayerTypes));
}

void SettingsManager::declareAction(const QString &name, const QString &text, const QKeySequence &defaultShortcut)
{
	declareAction(name, ActionInfo(text, defaultShortcut));
}

QHash<QString, ToolInfo> SettingsManager::toolInfoHash() const { return d->toolDeclarationHash; }
QHash<QString, ActionInfo> SettingsManager::actionInfoHash() const { return d->actionDeclarationHash; }
QHash<QString, SideBarInfo> SettingsManager::sideBarInfoHash() const { return d->sideBarDeclarationHash; }
QHash<QString, ToolBarInfo> SettingsManager::toolBarInfoHash() const { return d->toolbarInfoHash; }
QHash<QString, MenuInfo> SettingsManager::menuInfoHash() const { return d->menuDeclarationHash; }

QStringList SettingsManager::toolNames() const { return d->toolDeclarationHash.keys(); }
QStringList SettingsManager::actionNames() const { return d->actionDeclarationHash.keys(); }
QStringList SettingsManager::sidebarNames() const { return d->sideBarDeclarationHash.keys(); }
QStringList SettingsManager::toolbarNames() const { return d->toolbarInfoHash.keys(); }
QStringList SettingsManager::menuNames() const { return d->menuDeclarationHash.keys(); }

void SettingsManager::applyKeyBindingsToActionDeclarations()
{
	QVariantMap keyBindMap = d->settings[".key-bindings"].toMap();
	
	for (auto keyIter = keyBindMap.begin(); keyIter != keyBindMap.end(); ++keyIter)
	{
		for (auto actionIter = d->actionDeclarationHash.begin(); actionIter != d->actionDeclarationHash.end(); ++actionIter)
		{
			if (actionIter.key() == keyIter.key())
				actionIter->shortcut = keyIter.value().toString();
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
