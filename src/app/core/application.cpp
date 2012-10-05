#include <QtGui>
#include "QJson/Parser"

#include "application.h"

namespace PaintField
{

QVariant loadJsonFromFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << Q_FUNC_INFO << ": failed to open file";
		return;
	}
	
	QJson::Parser parser;
	return parser.parse(&file);
}

Application::Application(int &argv, char **args) :
    TabletApplication(argv, args)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	declareMenuItemTitle("file", tr("File"));
	
	_workspaceManager = new WorkspaceManager(this);
	_workspaceManager->newWorkspace();
	
	_actionManager = new ActionManager(this);
}

void Application::addToolFactory(ToolFactory *factory)
{
	if (factory)
	{
		factory->setParent(this);
		_toolFactories << factory;
	}
}

void Application::loadShortcutsFromJson(const QString &path)
{
	QVariant json = loadJsonFromFile(path);
	QVariantList maps = json.toList();
	
	for (const QVariant &mapData : maps)
	{
		QVariantMap map = mapData.toMap();
	}
}

}
