#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <QTest>
#include <QObject>
#include "paintfield-core/util.h"

using namespace PaintField;

class AutoTest
{
public:
	AutoTest() {}
	
	void addTestObject(QObject *obj)
	{
		_objects << obj;
	}
	
	template <class T>
	void addTestClass()
	{
		QObject *obj = new T;
		obj->setObjectName(T::staticMetaObject.className());
		addTestObject(obj);
	}
	
	int run()
	{
		int r = 0;
		for (QObject *obj : _objects)
			r += QTest::qExec(obj);
		return r;
	}
	
private:
	
	QObjectList _objects;
};

extern AutoTest autoTest;

template <class T>
class AutoTestRegisterer
{
public:
	
	AutoTestRegisterer()
	{
		autoTest.addTestClass<T>();
	}
};

#define AUTOTEST_ADD_CLASS(CLASS) static AutoTestRegisterer<CLASS> t;

#endif // AUTOTEST_H
