#pragma once

#include "abstractrectlayer.h"

namespace PaintField {

class TextLayer : public AbstractRectLayer
{
public:
	
	typedef AbstractRectLayer super;
	
	TextLayer() : super() {}
	
	Layer *createAnother() const override { return new TextLayer(); }
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	QVariantMap saveProperies() const override;
	void loadProperties(const QVariantMap &map) override;
	
	void setText(const QString &text);
	QString text() const { return _text; }
	
	void setFont(const QFont &font);
	QFont font() const { return _font; }
	
	bool includes(const QPoint &pos, int margin) const override;
	
protected:
	
	void updateFillPath() override;
	
private:
	
	QString _text;
	QFont _font;
};

} // namespace PaintField

