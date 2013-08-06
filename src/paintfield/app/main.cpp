#include <QTextCodec>
#include <QDir>
#include <QTranslator>
#include "paintfield/core/application.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/extensionmanager.h"
#include "paintfield/extensions/rootextensionfactory.h"

#define QUOTE(x) #x
#define DEFINE_STR(x) QUOTE(x)

using namespace PaintField;

int main(int argc, char *argv[])
{
#ifdef PF_FORCE_RASTER_ENGINE
	QApplication::setGraphicsSystem("raster");
#endif
	
	Application a(argc, argv);
	a.setApplicationVersion(DEFINE_STR(PF_VERSION));
	
	QDir applicationDir(qApp->applicationDirPath());
	
	// load translations
	for (const auto &module : {"core", "extensions", "qt_help", "qt"})
	{
		auto translator = new QTranslator();
		translator->load(QLocale::system(), module, "_", applicationDir.filePath("Translations"), ".qm");
		a.installTranslator(translator);
	}
	
#ifdef Q_OS_WIN
	qApp->addLibraryPath(applicationDir.filePath("plugins"));
#endif
	
	QString message;
	if (argc >= 2)
		message = argv[1];
	
	if (a.isRunning())
	{
		a.sendMessage(message);
		return 0;
	}
	
	AppController appCon(&a);
	appCon.extensionManager()->addExtensionFactory(new RootExtensionFactory);
	appCon.begin();
	
	return a.exec();
}
