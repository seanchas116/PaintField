#pragma once

#include "global.h"
#include <QIODevice>

namespace PaintField {

class ZipFile;

class ZipArchive
{
public:
	
	friend class ZipFile;
	
	explicit ZipArchive(QIODevice *device);
	~ZipArchive();
	
	bool open();
	void close();
	
	bool isOpen() const;
	
private:
	
	struct Data;
	Data *d;
};

class ZipFile : public QIODevice
{
public:
	
	ZipFile(ZipArchive *archive);
	ZipFile(ZipArchive *archive, const QString &filepath);
	
	~ZipFile();
	
	bool open();
	bool open(const QString &filepath);
	void close() override;
	
	void setNoCompression(bool noCompression);
	
protected:
	
	qint64 readData(char *data, qint64 maxSize) override;
	qint64 writeData(const char *data, qint64 maxSize) override;
	
private:
	
	bool open(OpenMode mode) override;
	
	struct Data;
	Data *d;
};

class UnzipArchive
{
public:
	
	friend class UnzipFile;
	
	explicit UnzipArchive(QIODevice *device);
	~UnzipArchive();
	
	bool open();
	void close();
	
	bool isOpen() const;
	
private:
	
	struct Data;
	Data *d;
};

class UnzipFile : public QIODevice
{
public:
	
	UnzipFile(UnzipArchive *archive);
	UnzipFile(UnzipArchive *archive, const QString &filepath);
	
	~UnzipFile();
	
	bool open(const QString &filepath);
	bool open();
	
	void close() override;
	
	bool isSequential() const override { return true; }
	bool atEnd() const override;
	qint64 pos() const override;
	qint64 size() const override;
	
protected:
	
	qint64 readData(char *data, qint64 maxSize) override;
	qint64 writeData(const char *data, qint64 maxSize) override;
	
private:
	
	bool open(OpenMode mode) override;
	
	struct Data;
	Data *d;
};

} // namespace PaintField
