#include <QTextCodec>
#include <QDir>
#include "autotest.h"

using namespace PaintField;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	autoTest()->run();
	
	return 0;
}
