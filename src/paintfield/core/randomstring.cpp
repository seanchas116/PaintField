#include <QDebug>
#include <QDir>
#include <ctime>

#include "randomstring.h"

namespace PaintField
{

QString randomString(int length)
{
	qsrand(time(0));
	
	QString str;
	
	for (int i = 0; i < length; ++i)
		str += QString::number(qrand() % 36, 36);
	
	return str;
}

QString createTemporaryFilePath()
{
	QDir tempDir = QDir::temp();
	QString fileName;
	
	forever {
		fileName = "pfield_temp." + randomString(6);
		if (!tempDir.exists(fileName))
			break;
	}
	
	qDebug() << Q_FUNC_INFO << ": created" << tempDir.absoluteFilePath(fileName);
	
	return tempDir.absoluteFilePath(fileName);
}

}

