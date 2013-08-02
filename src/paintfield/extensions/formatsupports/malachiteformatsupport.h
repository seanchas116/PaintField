#pragma once
#include "paintfield/core/singlelayerformatsupport.h"

namespace PaintField {

/**
 * MalachiteFormatSupport provides support for image file formats supported by Malachite (via FreeImage).
 */
class MalachiteFormatSupport : public SingleLayerFormatSupport
{
	Q_OBJECT
public:
	explicit MalachiteFormatSupport(QObject *parent = 0);
	
	bool readSingleLayer(QIODevice *device, Malachite::Surface *surface, QSize *size) override;
	bool writeSingleLayer(QIODevice *device, const Malachite::Surface &surface, const QSize &size, const QVariant &option) override;
	
	virtual QString malachiteFormat() const = 0;
	
signals:
	
public slots:
	
private:
};

class JpegFormatSupport : public MalachiteFormatSupport
{
	Q_OBJECT
	
public:
	
	explicit JpegFormatSupport(QObject *parent = 0);
	
	QStringList suffixes() const override { return { "jpg", "jpeg" }; }
	QString name() const override { return "paintfield-format-jpeg"; }
	
	bool canRead() const override { return true; }
	bool canWrite() const override { return true; }
	
	Capabilities capabilities() const override { return 0; }
	
	QWidget *createExportingOptionWidget() override;
	QVariant exportingOptionForWidget(QWidget *widget) override;
	
	QString malachiteFormat() const override { return "jpeg"; }
};

class PngFormatSupport : public MalachiteFormatSupport
{
	Q_OBJECT
	
public:
	
	explicit PngFormatSupport(QObject *parent = 0);
	
	QStringList suffixes() const override { return { "png" }; }
	QString name() const override { return "paintfield-format-png"; }
	
	bool canRead() const override { return true; }
	bool canWrite() const override { return true; }
	
	Capabilities capabilities() const override { return CapabilityLossless | CapabilityAlphaChannel; }
	
	QWidget *createExportingOptionWidget() override;
	QVariant exportingOptionForWidget(QWidget *widget) override;
	
	QString malachiteFormat() const override { return "png"; }
};

} // namespace PaintField
