#include <QGraphicsSimpleTextItem>
#include "pathrecorder.h"
#include "serializationutil.h"

#include "textlayer.h"

namespace PaintField {

bool TextLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
		case RoleText:
			setText(data.toString());
			return true;
		case RoleFont:
			setFont(data.value<QFont>());
			return true;
		case RoleTextAlignment:
			setAlignment(Qt::Alignment(data.toInt()));
			return true;
		default:
			return super::setProperty(data, role);
	}
}

QVariant TextLayer::property(int role) const
{
	switch (role)
	{
		case RoleText:
			return _text;
		case RoleFont:
			return _font;
		case RoleTextAlignment:
			return int(_alignment);
		default:
			return super::property(role);
	}
}

void TextLayer::encode(QDataStream &stream) const
{
	super::encode(stream);
	stream << _text << SerializationUtil::mapFromFont(_font) << int(_alignment);
}

void TextLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	
	QVariantMap fontMap;
	int alignmentInt;
	stream >> _text >> fontMap >> alignmentInt;
	_font = SerializationUtil::fontFromMap(fontMap);
	_alignment = Qt::Alignment(alignmentInt);
	
	updateFillPath();
}

QVariantMap TextLayer::saveProperies() const
{
	auto map = super::saveProperies();
	
	map["text"] = _text;
	map["font"] = SerializationUtil::mapFromFont(_font);
	map["alignment"] = SerializationUtil::mapFromAlignment(_alignment);
	
	return map;
}

void TextLayer::loadProperties(const QVariantMap &map)
{
	super::loadProperties(map);
	
	_text = map["text"].toString();
	_font = SerializationUtil::fontFromMap(map["font"].toMap());
	_alignment = SerializationUtil::alignmentFronMap(map["alignment"].toMap());
	
	updateFillPath();
}

void TextLayer::setText(const QString &text)
{
	_text = text;
	updateFillPath();
}

void TextLayer::setFont(const QFont &font)
{
	_font = font;
	updateFillPath();
}

void TextLayer::setAlignment(Qt::Alignment alignment)
{
	PAINTFIELD_DEBUG << alignment;
	_alignment = alignment;
	updateFillPath();
}

bool TextLayer::includes(const QPoint &pos, int margin) const
{
	return rect().adjusted(-margin, -margin, margin, margin).contains(pos);
}

void TextLayer::updateFillPath()
{
	PathRecorder recorder;
	QPainter painter(&recorder);
	
	auto font = _font;
	font.setPixelSize(font.pointSize());
	
	QTextOption option;
	option.setWrapMode(QTextOption::WrapAnywhere);
	option.setAlignment(_alignment);
	
	painter.setFont(font);
	painter.drawText(rect(), _text, option);
	
	QPainterPath rectPath;
	rectPath.addRect(rect());
	
	setFillPath((recorder.path() & rectPath).simplified());
}

} // namespace PaintField
