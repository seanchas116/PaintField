#include <QtGui>

#include "paintfield-core/application.h"
#include "paintfield-core/appcontroller.h"
#include "paintfield-extension/extensionmodulefactory.h"

using namespace PaintField;

int main(int argc, char *argv[])
{
	Application a(argc, argv);
	
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
	
	QObject::connect(&a, SIGNAL(messageReceived(QString)), &appCon, SLOT(handleMessage(QString)));
	appCon.addModuleFactory(new ExtensionModuleFactory);
	
	appCon.begin();
	
	return a.exec();
}
