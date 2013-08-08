#pragma once

#include "paintfield/core/formatsupport.h"

namespace PaintField {

class OpenRasterFormatSupport : public FormatSupport
{
	Q_OBJECT
public:
	explicit OpenRasterFormatSupport(QObject *parent = 0);
	
	QStringList suffixes() const override { return  { "ora" }; }
	QString name() const override { return "paintfield.format.openRaster"; }
	bool canRead() const override { return true; }
	bool canWrite() const override { return false; }
	
	Capabilities capabilities() const override { return CapabilityAll; }
	
	bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) override;
	bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option) override;
};

}
