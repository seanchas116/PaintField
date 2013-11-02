#pragma once

#include "abstractrectlayer.h"

namespace PaintField {

class TextLayer : public AbstractRectLayer
{
public:
	
	typedef AbstractRectLayer super;
	
	TextLayer() : super() {}
	
	LayerRef createAnother() const override { return makeSP<TextLayer>(); }
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	QVariantMap saveProperties() const override;
	void loadProperties(const QVariantMap &map) override;
	
	void setText(const QString &text);
	QString text() const { return _text; }
	
	void setFont(const QFont &font);
	QFont font() const { return _font; }
	
	void setAlignment(Qt::Alignment alignment);
	Qt::Alignment alignment() const { return _alignment; }
	
	bool includes(const QPoint &pos, int margin) const override;
	
protected:
	
	void updateFillPath() override;
	
private:
	
	QString _text;
	QFont _font;
	Qt::Alignment _alignment = Qt::AlignLeft | Qt::AlignTop;
};

class TextLayerFactory : public LayerFactory
{
public:
	
	TextLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	LayerRef create() const override;
	const std::type_info &typeInfo() const override;
};

} // namespace PaintField

