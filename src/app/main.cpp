#include <QtGui>

#include "core/application.h"

int main(int argc, char *argv[])
{
	PaintField::Application application(argc, argv);
	
	return application.exec();
}
