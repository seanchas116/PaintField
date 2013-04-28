#include "serializationutil.h"

namespace PaintField {

namespace SerializationUtil {

using namespace Malachite;

QVariantMap mapFromColor(const Color &c)
{
	QVariantMap map;
	map["a"] = c.alpha();
	map["r"] = c.red();
	map["g"] = c.green();
	map["b"] = c.blue();
	return map;
}

Color colorFromMap(const QVariantMap &map)
{
	return Color::fromRgbValue(map["r"].toDouble(), map["g"].toDouble(), map["b"].toDouble(), map["a"].toDouble());
}

QVariantMap mapFromBrush(const Brush &brush)
{
	QVariantMap map;
	
	switch (brush.type())
	{
		// TODO: other types
		default:
		case BrushTypeColor:
		{
			map["type"] = "color";
			map["color"] = mapFromColor(brush.color());
			break;
		}
	}
	
	return map;
}

Brush brushFromMap(const QVariantMap &map)
{
	auto type = map["type"].toString();
	
	if (type == "color")
		return colorFromMap(map["color"].toMap());
	else
		return Brush();
}

QVariantMap mapFromFont(const QFont &font)
{
	QVariantMap map;
	map["family"] = font.family();
	map["style"] = font.styleName();
	map["italic"] = font.italic();
	map["pointSize"] = font.pointSize();
	return map;
}

QFont fontFromMap(const QVariantMap &map)
{
	QFont font;
	font.setFamily(map["family"].toString());
	font.setStyleName(map["style"].toString());
	font.setItalic(map["italic"].toBool());
	font.setPointSize(map["pointSize"].toInt());
	return font;
}

QVariantMap mapFromAlignment(Qt::Alignment alignment)
{
	QVariantMap map;
	QString vstring, hstring;
	
	if (alignment & Qt::AlignRight)
		hstring = "right";
	else if (alignment & Qt::AlignHCenter)
		hstring = "center";
	else
		hstring = "left";
	
	if (alignment & Qt::AlignBottom)
		vstring = "bottom";
	else if (alignment & Qt::AlignVCenter)
		vstring = "center";
	else
		vstring = "top";
	
	map["horizontal"] = hstring;
	map["vertical"] = vstring;
	
	return map;
}

Qt::Alignment alignmentFronMap(const QVariantMap &map)
{
	QString hstring = map["horizontal"].toString(), vstring = map["vertical"].toString();
	
	Qt::Alignment alignment = 0;
	
	if (hstring == "right")
		alignment |= Qt::AlignRight;
	else if (hstring == "center")
		alignment |= Qt::AlignHCenter;
	else
		alignment |= Qt::AlignLeft;
	
	if (vstring == "bottom")
		alignment |= Qt::AlignBottom;
	else if (vstring == "center")
		alignment |= Qt::AlignVCenter;
	else
		alignment |= Qt::AlignTop;
	
	return alignment;
}

}

} // namespace PaintField
