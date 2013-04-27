#include <QGraphicsSimpleTextItem>
#include "pathrecorder.h"

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
		default:
			return super::property(role);
	}
}

void TextLayer::encode(QDataStream &stream) const
{
	super::encode(stream);
	stream << _text << _font;
}

void TextLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	stream >> _text >> _font;
	updateFillPath();
}

QVariantMap TextLayer::saveProperies() const
{
	auto map = super::saveProperies();
	
	// TODO
	
	return map;
}

void TextLayer::loadProperties(const QVariantMap &map)
{
	super::loadProperties(map);
	
	// TODO
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
	
	painter.setFont(font);
	painter.drawText(rect(), _text, option);
	
	QPainterPath rectPath;
	rectPath.addRect(rect());
	
	setFillPath(recorder.path() & rectPath);
}

} // namespace PaintField
