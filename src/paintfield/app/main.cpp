#include <QTextCodec>
#include "paintfield/core/application.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/unittest/autotest.h"
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
	
#ifdef PF_TEST
	autoTest()->run();
	return 0;
#endif
	
	QString message;
	if (argc >= 2)
		message = argv[1];
	
	if (a.isRunning())
	{
		a.sendMessage(message);
		return 0;
	}
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	AppController appCon(&a);
	appCon.extensionManager()->addExtensionFactory(new RootExtensionFactory);
	appCon.begin();
	
	return a.exec();
}
