#ifndef BRUSHTOOLMODULE_H
#define BRUSHTOOLMODULE_H

#include "paintfield-core/smartpointer.h"
#include "paintfield-core/module.h"

namespace PaintField {

class BrushSettingSidebar;
class BrushPresetManager;
class BrushStrokerFactory;
class BrushStrokerFactoryManager;
class BrushPreferencesManager;

class BrushToolExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	BrushToolExtension(Workspace *workspace, QObject *parent);
	
	BrushPresetManager *presetManager() { return _presetManager; }
	BrushStrokerFactoryManager *sourceFactoryManager() { return _strokerFactoryManager; }
	
	Tool *createTool(const QString &name, Canvas *parent) override;
	
signals:
	
	void strokerFactoryChanged(BrushStrokerFactory *factory);
	
private slots:
	
	void onStrokerChanged(const QString &name);
	
private:
	
	BrushPresetManager *_presetManager = 0;
	BrushStrokerFactoryManager *_strokerFactoryManager = 0;
	BrushPreferencesManager *_preferencesManager = 0;
};

class BrushToolExtensionFactory : public ExtensionFactory
{
public:
	
	explicit BrushToolExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	WorkspaceExtensionList createWorkspaceExtensions(Workspace *workspace, QObject *parent) override
	{
		return { new BrushToolExtension(workspace, parent) };
	}
};

}

#endif // BRUSHTOOLMODULE_H
