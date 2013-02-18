#include <QTextCodec>
#include "paintfield/core/application.h"
#include "paintfield/core/appcontroller.h"

#define QUOTE(x) #x
#define DEFINE_STR(x) QUOTE(x)

using namespace PaintField;

int main(int argc, char *argv[])
{
	Application a(argc, argv);
	a.setApplicationVersion(DEFINE_STR(PF_VERSION));
	
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
	appCon.begin();
	
	return a.exec();
}
