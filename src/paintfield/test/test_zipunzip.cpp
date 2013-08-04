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
		ZipArchive zipArchive(path);
		zipArchive.open();
		
		ZipFile file(&zipArchive, "test.txt");
		file.open();
		
		QTextStream stream(&file);
		stream << testString;
	}
	
	{
		UnzipArchive unzArchive(path);
		unzArchive.open();
		
		UnzipFile file(&unzArchive, "test.txt");
		file.open();
		
		QTextStream stream(&file);
		
		QCOMPARE(stream.readAll(), testString);
	}
}

PF_ADD_TESTCLASS(Test_ZipUnzip)

}

