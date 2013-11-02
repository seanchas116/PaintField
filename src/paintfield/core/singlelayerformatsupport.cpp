#include "rasterlayer.h"
#include "layerrenderer.h"

#include "singlelayerformatsupport.h"

namespace PaintField {

SingleLayerFormatSupport::SingleLayerFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
}

bool SingleLayerFormatSupport::read(QIODevice *device, QList<LayerRef> *layers, QSize *size)
{
	Malachite::Surface surface;
	
	if (!readSingleLayer(device, &surface, size))
		return false;
	
	auto layer = makeSP<RasterLayer>();
	layer->setSurface(surface);
	
	*layers = {layer};
	
	return true;
}

bool SingleLayerFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	LayerRenderer renderer;
	auto surface = renderer.renderToSurface(layers, Malachite::Surface::rectToKeys(QRect(QPoint(), size)));
	return writeSingleLayer(device, surface, size, option);
}

bool SingleLayerFormatSupport::readSingleLayer(QIODevice *device, Malachite::Surface *surface, QSize *size)
{
	Q_UNUSED(device);
	Q_UNUSED(surface);
	Q_UNUSED(size);
	return false;
}

bool SingleLayerFormatSupport::writeSingleLayer(QIODevice *device, const Malachite::Surface &surface, const QSize &size, const QVariant &option)
{
	Q_UNUSED(device);
	Q_UNUSED(surface);
	Q_UNUSED(size);
	Q_UNUSED(option);
	return false;
}

} // namespace PaintField
