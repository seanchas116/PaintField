#include <QtGui>

#include "fsapplication.h"

int main(int argc, char *argv[])
{
	FSApplication application(argc, argv);
	
	return application.exec();
}
