#include <QDebug>
#include <QDir>

#include "fsrandomstring.h"

QString fsRandomString(int length)
{
	qsrand(time(0));
	
	QByteArray source;
	int sourceLength = length * 6 / 8 + 1;
	source.reserve(sourceLength);
	for (int i = 0; i < sourceLength; ++i) {
		source += uint8_t(qrand() % 256);
	}
	
	QString result(source.toBase64());
	return result.left(length);
}

QString fsCreateTemporaryFilePath()
{
	QDir tempDir = QDir::temp();
	QString fileName;
	
	forever {
		fileName = "azurite_temp." + fsRandomString(6);
		if (!tempDir.exists(fileName))
			break;
	}
	
	qDebug() << "fsCreateTemporaryFilePath: created" << tempDir.absoluteFilePath(fileName);
	
	return tempDir.absoluteFilePath(fileName);
}


