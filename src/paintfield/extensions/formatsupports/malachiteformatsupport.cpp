#include <Malachite/ImageIO>

#include "jpegexportform.h"
#include "pngexportform.h"

#include "malachiteformatsupport.h"

namespace PaintField {

MalachiteFormatSupport::MalachiteFormatSupport(QObject *parent) :
	SingleLayerFormatSupport(parent)
{
}

bool MalachiteFormatSupport::readSingleLayer(QIODevice *device, Malachite::Surface *surface, QSize *size)
{
	Malachite::ImageImporter importer;
	if (!importer.load(device))
		return false;
	
	*surface = importer.toSurface();
	*size = importer.size();
	return true;
}

bool MalachiteFormatSupport::writeSingleLayer(QIODevice *device, const Malachite::Surface &surface, const QSize &size)
{
	auto settings = this->settings();
	bool hasAlpha = settings.value("hasAlpha", true).toBool();
	int quality = settings.value("quality", 100).toInt();
	
	Malachite::ImageExporter exporter(this->malachiteFormat(), hasAlpha);
	exporter.setSurface(surface, size);
	exporter.setQuality(quality);
	
	return exporter.save(device);
}

JpegFormatSupport::JpegFormatSupport(QObject *parent) :
	MalachiteFormatSupport(parent)
{
	setShortDescription(tr("JPEG"));
}

bool JpegFormatSupport::hasCapability(Capability capability) const
{
	switch (capability)
	{
		case Capability::AlphaChannel:
			return false;
		case Capability::Layers:
			return false;
		case Capability::Lossless:
			return false;
		default:
			return false;
	}
}

QWidget *JpegFormatSupport::createExportOptionWidget()
{
	return new JpegExportForm();
}

void JpegFormatSupport::setExportOptions(QWidget *widget)
{
	auto form = dynamic_cast<JpegExportForm *>(widget);
	if (!form)
		return;
	
	QVariantHash hash;
	hash["quality"] = form->quality();
	
	setSettings(hash);
}

PngFormatSupport::PngFormatSupport(QObject *parent) :
	MalachiteFormatSupport(parent)
{
	setShortDescription(tr("PNG"));
}

bool PngFormatSupport::hasCapability(Capability capability) const
{
	switch (capability)
	{
		case Capability::AlphaChannel:
			return true;
		case Capability::Layers:
			return false;
		case Capability::Lossless:
			return true;
		default:
			return false;
	}
}

QWidget *PngFormatSupport::createExportOptionWidget()
{
	return new PngExportForm();
}

void PngFormatSupport::setExportOptions(QWidget *widget)
{
	auto form = dynamic_cast<PngExportForm *>(widget);
	if (!form)
		return;
	
	QVariantHash hash;
	hash["hasAlpha"] = form->isAlphaEnabled();
	
	setSettings(hash);
}

} // namespace PaintField
