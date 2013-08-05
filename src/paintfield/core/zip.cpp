#include <QFile>
#include "libs/minizip/unzip.h"
#include "libs/minizip/zip.h"

#include "zip.h"

namespace PaintField {

namespace MinizipSupport
{

static voidpf openFile(voidpf opaque, const void *filename, int mode)
{
	Q_UNUSED(filename);
	
	auto ioDevice = static_cast<QIODevice *>(opaque);
	
	QIODevice::OpenMode flags;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
		flags = QIODevice::ReadOnly;
	else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
		flags = QIODevice::ReadWrite;
	else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
		flags = QIODevice::WriteOnly;
	else
		return nullptr;
	
	PAINTFIELD_DEBUG << ioDevice->openMode() << flags;
	
	if (ioDevice->openMode() == QIODevice::NotOpen)
	{
		if (!ioDevice->open(flags))
		{
			PAINTFIELD_WARNING << "cannot open io device";
			return nullptr;
		}
	}
	else
	{
		if (ioDevice->openMode() != flags)
		{
			PAINTFIELD_WARNING << "wrong open mode";
			return nullptr;
		}
	}
	
	return ioDevice;
}

static uLong readFile(voidpf opaque, voidpf stream, void *buf, uLong size)
{
	Q_UNUSED(stream);
	auto ioDevice = static_cast<QIODevice *>(opaque);
	return ioDevice->read(reinterpret_cast<char *>(buf), size);
}

static uLong writeFile(voidpf opaque, voidpf stream, const void *buf, uLong size)
{
	Q_UNUSED(stream);
	auto ioDevice = static_cast<QIODevice *>(opaque);
	return ioDevice->write(reinterpret_cast<const char *>(buf), size);
}

static ZPOS64_T tellFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(stream);
	auto ioDevice = static_cast<QIODevice *>(opaque);
	return ioDevice->pos();
}

static long seekFile(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
{
	Q_UNUSED(stream);
	auto ioDevice = static_cast<QIODevice *>(opaque);
	
	uint64_t pos;
	switch (origin)
	{
		case ZLIB_FILEFUNC_SEEK_CUR:
			pos = ioDevice->pos() + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_END:
			pos = ioDevice->size() + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_SET:
			pos = offset;
			break;
		default:
			return -1;
	}
	
	if (ioDevice->seek(pos))
		return 0;
	else
		return -1;
}

static int closeFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(stream);
	auto ioDevice = static_cast<QIODevice *>(opaque);
	ioDevice->close();
	return 0;
}

static int errorFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(opaque);
	Q_UNUSED(stream);
	return 0;
}

static void setFileFuncs(zlib_filefunc64_def &def, QIODevice *device)
{
	def.zopen64_file = openFile;
	def.zread_file = readFile;
	def.zwrite_file = writeFile;
	def.ztell64_file = tellFile;
	def.zseek64_file = seekFile;
	def.zclose_file = closeFile;
	def.zerror_file = errorFile;
	def.opaque = device;
}

}

struct ZipArchive::Data
{
	zipFile zip = 0;
	ZipFile *currentZipFile = 0;
	zlib_filefunc64_def fileFuncs;
};

ZipArchive::ZipArchive(QIODevice *device) :
	d(new Data)
{
	MinizipSupport::setFileFuncs(d->fileFuncs, device);
}

ZipArchive::~ZipArchive()
{
	if (isOpen())
		close();
	
	delete d;
}

bool ZipArchive::open()
{
	if (isOpen())
		return true;
	
	auto zip = zipOpen2_64(nullptr, APPEND_STATUS_CREATE, nullptr, &d->fileFuncs);
	d->zip = zip;
	return zip;
}

void ZipArchive::close()
{
	if (!isOpen())
		return;
	
	zipClose(d->zip, 0);
	d->zip = 0;
}

bool ZipArchive::isOpen() const
{
	return d->zip;
}

struct ZipFile::Data
{
	ZipArchive *archive;
	QString filepath;
	
	bool noCompression = false;
};

ZipFile::ZipFile(ZipArchive *archive) :
	d(new Data)
{
	d->archive = archive;
}

ZipFile::ZipFile(ZipArchive *archive, const QString &filepath) :
	ZipFile(archive)
{
	d->filepath = filepath;
}

ZipFile::~ZipFile()
{
	if (openMode() != NotOpen)
		close();
	
	delete d;
}

void ZipFile::setNoCompression(bool noCompression)
{
	d->noCompression = noCompression;
}

bool ZipFile::open()
{
	if (d->filepath.isEmpty())
	{
		PAINTFIELD_WARNING << "file path not specified";
		return false;
	}
	
	if (d->archive->d->currentZipFile)
	{
		PAINTFIELD_WARNING << "another ZipFile already opened";
		return false;
	}
	
	if (zipOpenNewFileInZip64(d->archive->d->zip, d->filepath.toLocal8Bit(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, d->noCompression ? Z_NO_COMPRESSION : Z_DEFAULT_COMPRESSION, 1) != ZIP_OK)
	{
		PAINTFIELD_WARNING << "cannot open file in archive";
		return false;
	}
	
	setOpenMode(WriteOnly);
	return true;
}

bool ZipFile::open(const QString &filepath)
{
	d->filepath = filepath;
	return open();
}

void ZipFile::close()
{
	if (openMode() == NotOpen)
		return;
	
	zipCloseFileInZip(d->archive->d->zip);
	d->archive->d->currentZipFile = 0;
	setOpenMode(NotOpen);
}

qint64 ZipFile::readData(char *data, qint64 maxSize)
{
	Q_UNUSED(data);
	Q_UNUSED(maxSize);
	
	PAINTFIELD_WARNING << "cannot read from ZipFile";
	return 0;
}

qint64 ZipFile::writeData(const char *data, qint64 maxSize)
{
	if (openMode() == NotOpen)
		return 0;
	
	if (zipWriteInFileInZip(d->archive->d->zip, data, maxSize) != ZIP_OK)
		return 0;
	return maxSize;
}

bool ZipFile::open(OpenMode mode)
{
	Q_UNUSED(mode);
	return false;
}

struct UnzipArchive::Data
{
	unzFile unzip = 0;
	UnzipFile *currentUnzipFile = 0;
	zlib_filefunc64_def fileFuncs;
};

UnzipArchive::UnzipArchive(QIODevice *device) :
	d(new Data)
{
	MinizipSupport::setFileFuncs(d->fileFuncs, device);
}

UnzipArchive::~UnzipArchive()
{
	if (isOpen())
		close();
	
	delete d;
}

bool UnzipArchive::open()
{
	if (isOpen())
		return true;
	
	d->unzip = unzOpen2_64(nullptr, &d->fileFuncs);
	return d->unzip;
}

void UnzipArchive::close()
{
	if (!isOpen())
		return;
	
	unzClose(d->unzip);
	d->unzip = 0;
}

bool UnzipArchive::isOpen() const
{
	return d->unzip;
}

struct UnzipFile::Data
{
	UnzipArchive *archive = 0;
	QString filepath;
	qint64 size = 0;
};

UnzipFile::UnzipFile(UnzipArchive *archive) :
	d(new Data)
{
	d->archive = archive;
}

UnzipFile::UnzipFile(UnzipArchive *archive, const QString &filepath) :
	UnzipFile(archive)
{
	d->filepath = filepath;
}

UnzipFile::~UnzipFile()
{
	if (openMode() != NotOpen)
		close();
	
	delete d;
}

bool UnzipFile::open(const QString &filepath)
{
	d->filepath = filepath;
	return open();
}

bool UnzipFile::open()
{
	if (d->filepath.isEmpty())
	{
		PAINTFIELD_WARNING << "file path not specified";
		return false;
	}
	
	if (d->archive->d->currentUnzipFile)
	{
		PAINTFIELD_WARNING << "another unzip file is open";
		return false;
	}
	
	auto unzip = d->archive->d->unzip;
	
	if (unzLocateFile(unzip, d->filepath.toUtf8(), 1) != UNZ_OK)
	{
		PAINTFIELD_WARNING << "file not found";
		return false;
	}
	
	unz_file_info64 fileInfo;
	unzGetCurrentFileInfo64(unzip, &fileInfo, 0, 0, 0, 0, 0, 0);
	d->size = fileInfo.uncompressed_size;
	
	if (unzOpenCurrentFile(unzip) != UNZ_OK)
	{
		PAINTFIELD_WARNING << "failed to open current file";
		return false;
	}
	
	setOpenMode(ReadOnly);
	return true;
}

void UnzipFile::close()
{
	if (openMode() == NotOpen)
		return;
	
	unzCloseCurrentFile(d->archive->d->unzip);
}

bool UnzipFile::atEnd() const
{
	return unzeof(d->archive->d->unzip);
}

qint64 UnzipFile::pos() const
{
	return unztell64(d->archive->d->unzip);
}

qint64 UnzipFile::size() const
{
	return d->size;
}

qint64 UnzipFile::readData(char *data, qint64 maxSize)
{
	if (openMode() == NotOpen)
		return 0;
	
	qint64 size = unzReadCurrentFile(d->archive->d->unzip, data, maxSize);
	if (size < 0)
		return 0;
	return size;
}

qint64 UnzipFile::writeData(const char *data, qint64 maxSize)
{
	Q_UNUSED(data);
	Q_UNUSED(maxSize);
	
	PAINTFIELD_WARNING << "cannot write into UnzipFile";
	
	return 0;
}

bool UnzipFile::open(OpenMode mode)
{
	Q_UNUSED(mode);
	return false;
}

} // namespace PaintField

