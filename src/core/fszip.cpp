#include <QtCore>
#include "fszip.h"


FSZip::FSZip(const QString &fileName, QObject *parent)
	: QObject(parent)
{
	open(fileName);
}

FSZip::FSZip(QObject *parent)
	: QObject(parent),
	  _zip(0)
{
}

FSZip::~FSZip()
{
	close();
}

bool FSZip::open(const QString &fileName, int flags, int *error)
{
	_zip = zip_open(fileName.toLocal8Bit().constData(), flags, error);
	return _zip;
}

void FSZip::close()
{
	if (_zip) {
		zip_close(_zip);
		_zip = 0;
	}
}

uint64_t FSZip::fileIndex(const QString &name)
{
	if (!_zip) {
		qWarning() << "FSZip::fileIndex: zip is not open";
		return 0;
	}
	
	return zip_name_locate(_zip, name.toLocal8Bit().constData(), 0);
}

FSZipFileInfo FSZip::fileInfo(const QString &name)
{
	FSZipFileInfo info;
	
	if (!_zip) {
		qWarning() << "FSZip::fileInfo: zip is not open";
		return info;
	}
	
	int result = zip_stat(_zip, name.toLocal8Bit().constData(), 0, &info._stat);
	if (result < 0)
		qWarning() << "FSZip::fileInfo: error";
	return info;
}

FSZipFileInfo FSZip::fileInfo(uint64_t index)
{
	FSZipFileInfo info;
	
	if (!_zip) {
		qWarning() << "FSZip::fileInfo: zip is not open";
		return info;
	}
	
	int result = zip_stat_index(_zip, index, 0, &info._stat);
	if (result < 0)
		qWarning() << "FSZip::fileInfo: error";
	return info;
}

QByteArray FSZip::readFile(uint64_t index)
{
	if (!_zip) {
		qWarning() << "FSZip::readFile: zip is not open";
		return QByteArray();
	}
	
	uint64_t size = fileInfo(index).size();
	
	zip_file *file = zip_fopen_index(_zip, index, 0);
	
	QByteArray byteArray;
	if (!file)
		return byteArray;
	
	byteArray.resize(size);
	uint64_t readSize = zip_fread(file, byteArray.data(), size);
	
	if (readSize != size) {
		qWarning() << "FSZip::readFile: error";
	}
	
	zip_fclose(file);
	
	return byteArray;
}

QByteArray FSZip::readFile(const QString &fileName)
{
	if (!_zip) {
		qWarning() << "FSZip::readFile: zip is not open";
		return QByteArray();
	}
	
	uint64_t size = fileInfo(fileName).size();
	
	zip_file *file = zip_fopen(_zip, fileName.toLocal8Bit().constData(), 0);
	
	QByteArray byteArray;
	if (!file)
		return byteArray;
	
	byteArray.resize(size);
	uint64_t readSize = zip_fread(file, byteArray.data(), size);
	
	if (readSize != size) {
		qWarning() << "FSZip::readFile: error";
	}
	
	zip_fclose(file);
	
	return byteArray;
}

bool FSZip::writeFile(const QString &fileName, const QByteArray &byteArray)
{
	if (!_zip) {
		qWarning() << "FSZip::writeFile: zip is not open";
		return false;
	}
	
	zip_source *source = zip_source_buffer(_zip, byteArray.constData(), byteArray.size(), 0);
	if (!source) {
		qWarning() << "FSZip::writeFile: error";
		return false;
	}
	
	if (zip_add(_zip, fileName.toLocal8Bit().constData(), source) < 0) {
		qWarning() << "FSZip::writeFile: error";
		zip_source_free(source);
		return false;
	}
	_zipSources << source;
	return true;
}

