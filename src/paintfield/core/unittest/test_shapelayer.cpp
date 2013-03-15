#ifdef PF_TEST

#include "autotest.h"
#include "paintfield/core/rectlayer.h"

#include "test_shapelayer.h"

using namespace Malachite;

namespace PaintField
{

Test_ShapeLayer::Test_ShapeLayer(QObject *parent) :
	QObject(parent)
{
}

void Test_ShapeLayer::test_encodeDecode()
{
	RectLayer l1, l2;
	l1.setStrokePosition(StrokePositionInside);
	l1.setStrokeWidth(2.0);
	l1.setJoinStyle(Qt::BevelJoin);
	l1.setCapStyle(Qt::FlatCap);
	l1.setFillEnabled(false);
	l1.setStrokeEnabled(false);
	
	Brush fillBrush = Color::fromRgbValue(0.1, 0.2, 0.3, 0.4);
	Brush strokeBrush = Color::fromRgbValue(0.5, 0.6, 0.7, 0.8);
	
	l1.setFillBrush(fillBrush);
	l1.setStrokeBrush(strokeBrush);
	
	QByteArray bytes;
	
	{
		QDataStream encoder(&bytes, QIODevice::WriteOnly);
		l1.encode(encoder);
	}
	{
		QDataStream decoder(&bytes, QIODevice::ReadOnly);
		l2.decode(decoder);
	}
	
	QCOMPARE(l2.strokePosition(), StrokePositionInside);
	QCOMPARE(l2.strokeWidth(), 2.0);
	QCOMPARE(l2.joinStyle(), Qt::BevelJoin);
	QCOMPARE(l2.capStyle(), Qt::FlatCap);
	QCOMPARE(l2.isFillEnabled(), false);
	QCOMPARE(l2.isStrokeEnabled(), false);
	QCOMPARE(l2.fillBrush().pixel(), fillBrush.pixel());
	QCOMPARE(l2.strokeBrush().pixel(), strokeBrush.pixel());
}

inline static bool approxEqual(double a, double b)
{
	return fabs(a - b) <= 0.01;
}

void Test_ShapeLayer::test_saveProperties()
{
	RectLayer l1;
	l1.setStrokePosition(StrokePositionInside);
	l1.setStrokeWidth(2.0);
	l1.setJoinStyle(Qt::BevelJoin);
	l1.setCapStyle(Qt::FlatCap);
	l1.setFillEnabled(false);
	l1.setStrokeEnabled(false);
	
	Brush fillBrush = Color::fromRgbValue(0.1, 0.2, 0.3, 0.4);
	Brush strokeBrush = Color::fromRgbValue(0.5, 0.6, 0.7, 0.8);
	
	l1.setFillBrush(fillBrush);
	l1.setStrokeBrush(strokeBrush);
	
	auto map = l1.saveProperies();
	
	QVariantMap strokeMap = map["stroke"].toMap();
	
	QCOMPARE(strokeMap["position"].toString(), QString("inside"));
	QCOMPARE(strokeMap["joinStyle"].toString(), QString("bevel"));
	QCOMPARE(strokeMap["capStyle"].toString(), QString("flat"));
	QCOMPARE(strokeMap["enabled"].toBool(), false);
	QVERIFY(approxEqual(strokeMap["width"].toDouble(), 2.0));
	
	{
		auto brushMap = strokeMap["brush"].toMap();
		QCOMPARE(brushMap["type"].toString(), QString("color"));
		
		auto colorMap = brushMap["color"].toMap();
		QVERIFY(approxEqual(colorMap["r"].toDouble(), 0.5));
		QVERIFY(approxEqual(colorMap["g"].toDouble(), 0.6));
		QVERIFY(approxEqual(colorMap["b"].toDouble(), 0.7));
		QVERIFY(approxEqual(colorMap["a"].toDouble(), 0.8));
	}
	
	auto fillMap = map["fill"].toMap();
	
	QCOMPARE(fillMap["enabled"].toBool(), false);
	
	{
		auto brushMap = fillMap["brush"].toMap();
		QCOMPARE(brushMap["type"].toString(), QString("color"));
		
		auto colorMap = brushMap["color"].toMap();
		QVERIFY(approxEqual(colorMap["r"].toDouble(), 0.1));
		QVERIFY(approxEqual(colorMap["g"].toDouble(), 0.2));
		QVERIFY(approxEqual(colorMap["b"].toDouble(), 0.3));
		QVERIFY(approxEqual(colorMap["a"].toDouble(), 0.4));
	}
}

void Test_ShapeLayer::test_loadProperties()
{
	QVariantMap map;
	
	{
		QVariantMap strokeMap;
		strokeMap["position"] = "inside";
		strokeMap["joinStyle"] = "bevel";
		strokeMap["enabled"] = false;
		strokeMap["width"] = 2.0;
		
		{
			QVariantMap brushMap;
			brushMap["type"] = "color";
			
			{
				QVariantMap colorMap;
				colorMap["r"] = 0.5;
				colorMap["g"] = 0.6;
				colorMap["b"] = 0.7;
				colorMap["a"] = 0.8;
				
				brushMap["color"] = colorMap;
			}
			
			strokeMap["brush"] = brushMap;
		}
		
		map["stroke"] = strokeMap;
	}
	
	{
		QVariantMap fillMap;
		fillMap["enabled"] = false;
		
		{
			QVariantMap brushMap;
			brushMap["type"] = "color";
			
			{
				QVariantMap colorMap;
				colorMap["r"] = 0.1;
				colorMap["g"] = 0.2;
				colorMap["b"] = 0.3;
				colorMap["a"] = 0.4;
				
				brushMap["color"] = colorMap;
			}
			
			fillMap["brush"] = brushMap;
		}
		
		map["fill"] = fillMap;
	}
	
	RectLayer l2;
	l2.loadProperties(map);
	
	QCOMPARE(l2.strokePosition(), StrokePositionInside);
	QVERIFY(approxEqual(l2.strokeWidth(), 2.0));
	QCOMPARE(l2.joinStyle(), Qt::BevelJoin);
	QCOMPARE(l2.capStyle(), Qt::FlatCap);
	QCOMPARE(l2.isFillEnabled(), false);
	QCOMPARE(l2.isStrokeEnabled(), false);
	
	QVERIFY(approxEqual(l2.fillBrush().color().red(), 0.1));
	QVERIFY(approxEqual(l2.fillBrush().color().green(), 0.2));
	QVERIFY(approxEqual(l2.fillBrush().color().blue(), 0.3));
	QVERIFY(approxEqual(l2.fillBrush().color().alpha(), 0.4));
	
	QVERIFY(approxEqual(l2.strokeBrush().color().red(), 0.5));
	QVERIFY(approxEqual(l2.strokeBrush().color().green(), 0.6));
	QVERIFY(approxEqual(l2.strokeBrush().color().blue(), 0.7));
	QVERIFY(approxEqual(l2.strokeBrush().color().alpha(), 0.8));
}

void Test_ShapeLayer::test_setFillPath()
{
	
}

PF_ADD_TESTCLASS(Test_ShapeLayer)

}

#endif
