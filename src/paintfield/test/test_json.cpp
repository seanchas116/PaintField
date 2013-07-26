#include "autotest.h"
#include "paintfield/core/json.h"

#include "test_json.h"


namespace PaintField
{

Test_Json::Test_Json(QObject *parent) :
    QObject(parent)
{
}

static QByteArray testString()
{
	return u8"{\"array\": [1, 2.5, true], \"string\": \"文字列\", \"map\": {\"Alice\": 1, \"Bob\": 2} }";
}

static QVariant testVariant()
{
	QVariantMap mainMap;
	
	// PaintField::Json read json int values as long long
	mainMap["array"] = QVariantList({qlonglong(1), 2.5, true});
	mainMap["string"] = u8"文字列";
	
	{
		QVariantMap map;
		map["Alice"] = qlonglong(1);
		map["Bob"] = qlonglong(2);
		mainMap["map"] = map;
	}
	
	return mainMap;
}

void Test_Json::test_readWrite()
{
	auto variant = Json::read(testString());
	auto correctVariant = testVariant();
	
	qDebug() << "correct variant:" << correctVariant;
	qDebug() << variant;
	QCOMPARE(variant, correctVariant);
	
	variant = Json::read(Json::write(variant));
	qDebug() << variant;
	QCOMPARE(variant, correctVariant);
}

PF_ADD_TESTCLASS(Test_Json)

}

