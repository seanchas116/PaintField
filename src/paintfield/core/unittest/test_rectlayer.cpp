#ifdef PF_TEST

#include "autotest.h"
#include "paintfield/core/rectlayer.h"

#include "test_rectlayer.h"

namespace PaintField
{

Test_RectLayer::Test_RectLayer(QObject *parent) :
	QObject(parent)
{
}

void Test_RectLayer::test_encodeDecode()
{
	RectLayer l1, l2;
	l1.setRect(QRectF(10,20,30,40));
	l1.setShapeType(AbstractRectLayer::ShapeTypeEllipse);
	
	QByteArray bytes;
	{
		QDataStream encoder(&bytes, QIODevice::WriteOnly);
		l1.encode(encoder);
	}
	{
		QDataStream decoder(&bytes, QIODevice::ReadOnly);
		l2.decode(decoder);
	}
	
	QCOMPARE(l1.rect(), l2.rect());
	QCOMPARE(l1.shapeType(), l2.shapeType());
}

void Test_RectLayer::test_saveProperties()
{
	RectLayer l1;
	l1.setRect(QRectF(10,20,30,40));
	l1.setShapeType(AbstractRectLayer::ShapeTypeEllipse);
	
	auto map = l1.saveProperies();
	
	QVariantMap rectMap;
	rectMap["x"] = 10.0;
	rectMap["y"] = 20.0;
	rectMap["width"] = 30.0;
	rectMap["height"] = 40.0;
	
	QCOMPARE(map["rect"].toMap(), rectMap);
	QCOMPARE(map["shapeType"].toString(), QString("ellipse"));
}

void Test_RectLayer::test_loadProperties()
{
	QVariantMap map;
	{
		QVariantMap rectMap;
		rectMap["x"] = 10.0;
		rectMap["y"] = 20.0;
		rectMap["width"] = 30.0;
		rectMap["height"] = 40.0;
		map["rect"] = rectMap;
	}
	map["shapeType"] = "ellipse";
	
	RectLayer l1;
	l1.loadProperties(map);
	
	QCOMPARE(l1.rect(), QRectF(10, 20, 30, 40));
	QCOMPARE(l1.shapeType(), AbstractRectLayer::ShapeTypeEllipse);
}

void Test_RectLayer::test_setRect()
{
	auto rect = QRectF(10, 20, 30, 40);
	
	RectLayer l1;
	l1.setRect(rect);
	
	QPainterPath path;
	path.addRect(rect);
	
	QCOMPARE(l1.rect(), rect);
	QCOMPARE(l1.fillPath(), path);
}

PF_ADD_TESTCLASS(Test_RectLayer)

}

#endif
