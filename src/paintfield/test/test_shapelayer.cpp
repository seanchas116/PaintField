#include "paintfield/core/rectlayer.h"

#include "test_shapelayer.h"

using namespace Malachite;

Test_ShapeLayer::Test_ShapeLayer(QObject *parent) :
	QObject(parent)
{
}

void Test_ShapeLayer::test_encodeDecode()
{
	RectLayer l1, l2;
	l1.setStrokePosition(StrokePositionInside);
	l1.setStrokeWidth(2.0);
	l1.setJoinStyle(Qt::MiterJoin);
	l1.setCapStyle(Qt::FlatCap);
	l1.setFillEnabled(false);
	l1.setStrokeEnabled(false);
	
	auto fillColor = Color::fromRgbValue(0.1, 0.2, 0.3, 0.4);
	auto strokeColor = Color::fromRgbValue(0.5, 0.6, 0.7, 0.8);
	
	l1.setFillBrush(fillColor);
	l1.setStrokeBrush(strokeColor);
	
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
	QCOMPARE(l2.joinStyle(), Qt::MiterJoin);
	QCOMPARE(l2.capStyle(), Qt::FlatCap);
	QCOMPARE(l2.isFillEnabled(), false);
	QCOMPARE(l2.isStrokeEnabled(), false);
	QCOMPARE(l2.fillBrush().color(), fillColor);
	QCOMPARE(l2.strokeBrush().color(), strokeColor);
}

void Test_ShapeLayer::test_saveProperties()
{
	RectLayer l1;
	l1.setStrokePosition(StrokePositionInside);
	l1.setStrokeWidth(2.0);
	l1.setJoinStyle(Qt::MiterJoin);
	l1.setCapStyle(Qt::FlatCap);
	l1.setFillEnabled(false);
	l1.setStrokeEnabled(false);
	
	auto fillColor = Color::fromRgbValue(0.1, 0.2, 0.3, 0.4);
	auto strokeColor = Color::fromRgbValue(0.5, 0.6, 0.7, 0.8);
	
	l1.setFillBrush(fillColor);
	l1.setStrokeBrush(strokeColor);
	
	auto map = l1.saveProperies();
	
	QVariantMap strokeMap;
	strokeMap["position"] = "inside";
	strokeMap["joinStyle"] = "miter";
	strokeMap["capStyle"] = "flat";
	strokeMap["enabled"] = false;
	
	{
		QVariantMap brushMap;
		brushMap["type"] = "color";
		
		{
			QVariantMap colorMap;
			colorMap["a"] = 0.4;
			colorMap["r"] = 0.1;
			colorMap["g"] = 0.2;
			colorMap["b"] = 0.3;
			brushMap["color"] = colorMap;
		}
		
		strokeMap["brush"] = brushMap;
	}
	
	QVariantMap fillMap;
	fillMap["enabled"] = false;
	
	{
		QVariantMap brushMap;
		brushMap["type"] = "color";
		
		{
			QVariantMap colorMap;
			colorMap["a"] = 0.8;
			colorMap["r"] = 0.5;
			colorMap["g"] = 0.6;
			colorMap["b"] = 0.7;
			brushMap["color"] = colorMap;
		}
		
		fillMap["brush"] = brushMap;
	}
	
	QCOMPARE(map["stroke"].toMap(), strokeMap);
	QCOMPARE(map["fill"].toMap(), fillMap);
}

void Test_ShapeLayer::test_loadProperties()
{
	
}

void Test_ShapeLayer::test_setFillPath()
{
	
}
