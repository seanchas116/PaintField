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
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	b->setValue(10);
	Property::bind(a, "value", b, "value");
	QCOMPARE(10, a->value());
	QCOMPARE(10, b->value());

	b->setValue(20);
	QCOMPARE(20, a->value());
	QCOMPARE(20, b->value());

	a->setValue(30);
	QCOMPARE(30, a->value());
	QCOMPARE(20, b->value());
}

void Test_Property::test_bindTransform()
{
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	b->setValue(10);
	Property::bind(a, "value", [](int bValue){return bValue * 2;}, b, "value");
	QCOMPARE(20, a->value());
	b->setValue(20);
	QCOMPARE(40, a->value());
}

void Test_Property::test_bindCustomProperty()
{
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	auto setA = [a](int x){
		a->setValue(x);
	};
	auto getB = [b](){
		return b->value();
	};
	Property::bind(customProperty(a, setA), customProperty(b, getB, SIGNAL(valueChanged(int))));
	b->setValue(20);
	QCOMPARE(20, a->value());
}

void Test_Property::test_sync()
{
	auto a = new QObject();
	auto b = new QObject();

	b->setObjectName("init");
	Property::sync(a, "objectName", b, "objectName");
	QCOMPARE(QString("init"), a->objectName());
	QCOMPARE(QString("init"), b->objectName());

	a->setObjectName("test");
	QCOMPARE(QString("test"), a->objectName());
	QCOMPARE(QString("test"), b->objectName());
}

void Test_Property::test_syncTransform()
{
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	Property::sync(
		a, "value", [](const int &x){return x * 2;},
		b, "value", [](int x){return x / 2;});
	a->setValue(10);
	QCOMPARE(5, b->value());
	b->setValue(10);
	QCOMPARE(20, a->value());
}

void Test_Property::test_syncCustomProperty()
{
	auto a = new QSpinBox();
	auto b = new QSpinBox();
	auto setA = [a](int x){
		a->setValue(x);
	};
	auto getA = [a](){
		return a->value();
	};
	Property::sync(customProperty(a, setA, getA, SIGNAL(valueChanged(int))), qtProperty(b, "value"));
	a->setValue(24);
	QCOMPARE(24, b->value());
	b->setValue(12);
	QCOMPARE(12, a->value());
}

PF_ADD_TESTCLASS(Test_Property)

} // namespace PaintField


