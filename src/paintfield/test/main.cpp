#include <QTextCodec>
#include <QDir>
#include "autotest.h"

using namespace PaintField;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	autoTest()->run();
	
	return 0;
}
