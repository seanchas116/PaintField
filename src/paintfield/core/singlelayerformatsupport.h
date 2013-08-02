#pragma once

#include "formatsupport.h"

namespace PaintField {

class SingleLayerFormatSupport : public FormatSupport
{
	Q_OBJECT
public:
	explicit SingleLayerFormatSupport(QObject *parent = 0);
	
	bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) override final;
	bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option) override final;
	
	virtual bool readSingleLayer(QIODevice *device, Malachite::Surface *surface, QSize *size);
	virtual bool writeSingleLayer(QIODevice *device, const Malachite::Surface &surface, const QSize &size, const QVariant &option);
	
signals:
	
public slots:
	
};

} // namespace PaintField

