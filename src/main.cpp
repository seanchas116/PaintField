#include <QtGui>

#include "fsglobal.h"
#include "fscore.h"
#include "fsguimain.h"
#include "fstableteventfilter.h"

#include "fsmainpanel.h"

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	FSTabletEventFilter filter;
	application.installEventFilter(&filter);
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	FSCore core;
	FSGuiMain guiMain;
	
	return application.exec();
}
