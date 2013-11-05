#pragma once

#include "paintfield/core/extension.h"

namespace PaintField {

class BrushPresetManager;
class BrushStrokerFactoryManager;
class BrushPresetDatabase;

class BrushToolExtension : public WorkspaceExtension
{
	Q_OBJECT
public:
	BrushToolExtension(BrushPresetDatabase *presetDatabase, Workspace *workspace, QObject *parent);
	
	BrushPresetManager *presetManager() { return mPresetManager; }
	BrushStrokerFactoryManager *sourceFactoryManager() { return mStrokerFactoryManager; }
	
	Tool *createTool(const QString &name, Canvas *parent) override;
	
signals:
	
private slots:
	
private:
	
	BrushPresetManager *mPresetManager = 0;
	BrushStrokerFactoryManager *mStrokerFactoryManager = 0;
};

class BrushToolExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
	
public:
	
	explicit BrushToolExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	~BrushToolExtensionFactory();
	
	void initialize(AppController *app) override;
	
	WorkspaceExtensionList createWorkspaceExtensions(Workspace *workspace, QObject *parent) override;

private:

	BrushPresetDatabase *mPresetDatabase = 0;
	QString mPresetFilePath;
};

}

