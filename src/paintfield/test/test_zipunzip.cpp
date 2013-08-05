#include "paintfield/core/randomstring.h"
#include "paintfield/core/zip.h"

#include "autotest.h"

#include "test_zipunzip.h"

namespace PaintField
{

Test_ZipUnzip::Test_ZipUnzip(QObject *parent) :
	QObject(parent)
{
}

void Test_ZipUnzip::zipUnzip()
{
	auto path = createTemporaryFilePath();
	
	QString testString = "Lorem ipsum dolor sit amet";
	
	{
		QFile file(path);
		
		ZipArchive zipArchive(&file);
		zipArchive.open();
		
		ZipFile zipFile(&zipArchive, "test.txt");
		zipFile.open();
		
		QTextStream stream(&zipFile);
		stream << testString;
	}
	
	{
		QFile file(path);
		
		UnzipArchive unzArchive(&file);
		unzArchive.open();
		
		UnzipFile unzipFile(&unzArchive, "test.txt");
		unzipFile.open();
		
		QTextStream stream(&unzipFile);
		
		QCOMPARE(stream.readAll(), testString);
	}
}

PF_ADD_TESTCLASS(Test_ZipUnzip)

}

