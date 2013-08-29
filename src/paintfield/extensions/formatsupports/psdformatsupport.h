#pragma once

#include "paintfield/core/formatsupport.h"

namespace PaintField {

class PsdFormatSupport : public FormatSupport
{
	Q_OBJECT
public:
	explicit PsdFormatSupport(QObject *parent = 0);
	
	QStringList suffixes() const override { return { "psd" }; }
	QString name() const override { return "paintfield.format.psd"; }
	bool canRead() const override { return true; }
	bool canWrite() const override { return true; }
	
	Capabilities capabilities() const override { return CapabilityAll; }
	
	bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) override;
	bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option) override;
	
signals:
	
public slots:
	
};

} // namespace PaintField

