#pragma once

#include <QTest>
#include <QObject>

namespace PaintField {

class AutoTest : public QObject
{
	Q_OBJECT
public:
	explicit AutoTest(QObject *parent = 0);
	
	static AutoTest *instance() { return _instance; }
	
	static void addTestObject(QObject *obj)
	{
		create();
		_instance->_objects << obj;
	}
	
	void run()
	{
		for (auto obj : _objects)
			QTest::qExec(obj);
	}
	
signals:
	
public slots:
	
private:
	
	static void create()
	{
		if (!_instance)
			_instance = new AutoTest();
	}
	
	QObjectList _objects;
	static AutoTest *_instance;
};

inline AutoTest *autoTest() { return AutoTest::instance(); }

template <class T>
class AutoTestRegisterer
{
public:
	AutoTestRegisterer()
	{
		QObject *obj = new T();
		obj->setObjectName(obj->metaObject()->className());
		AutoTest::addTestObject(obj);
	}
};

} // namespace PaintField

#define PF_ADD_TESTCLASS(CLASS) static AutoTestRegisterer<CLASS> _autoTestRegisterer;

