#ifndef FSZIP_H
#define FSZIP_H

#include <QObject>
#include <QDateTime>
#include <zip.h>

class FSZipFileInfo
{
public:
	QString fileName() const { return QString(_stat.name); }
	uint64_t index() const { return _stat.index; }
	uint64_t size() const { return _stat.size; }
	uint64_t compressedSize() const { return _stat.comp_size; }
	QDateTime lastModified() const { return QDateTime::fromTime_t(_stat.mtime); }
	
	struct zip_stat _stat;
};

class FSZip : public QObject
{
	Q_OBJECT
public:
	FSZip(const QString &fileName, QObject *parent = 0);
	FSZip(QObject *parent = 0);
	~FSZip();
	
	bool open(const QString &fileName, int flags = 0, int *error = 0);
	void close();
	
	uint64_t fileIndex(const QString &name);
	FSZipFileInfo fileInfo(const QString &name);
	FSZipFileInfo fileInfo(uint64_t index);
	
	QByteArray readFile(uint64_t index);
	QByteArray readFile(const QString &fileName);
	bool writeFile(const QString &fileName, const QByteArray &byteArray);
	
private:
	QString _fileName;
	zip *_zip;
	QList<zip_source *> _zipSources;
};

#endif // FSZIP_H
