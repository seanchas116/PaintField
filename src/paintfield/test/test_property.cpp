#include "test_property.h"
#include "autotest.h"
#include "paintfield/core/property.h"
#include <QSpinBox>

namespace PaintField {

Test_Property::Test_Property(QObject *parent) :
	QObject(parent)
{
}

void Test_Property::test_bind()
{
	auto a = new QObject();
	auto b = new QObject();
	Property::bind(a, "objectName", b, "objectName");
	a->setObjectName("test");
	QCOMPARE(QString("test"), b->objectName());
}

void Test_Property::test_bindTransform()
{
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	Property::bind(
		a, "value", [](const int &x){return x * 2;},
		b, "value", [](int x){return x / 2;});
	a->setValue(10);
	QCOMPARE(5, b->value());
	b->setValue(10);
	QCOMPARE(20, a->value());
}

PF_ADD_TESTCLASS(Test_Property)

} // namespace PaintField


