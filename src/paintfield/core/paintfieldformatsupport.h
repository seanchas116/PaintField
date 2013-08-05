#pragma once

#include "formatsupport.h"

namespace PaintField {

class PaintFieldFormatSupport : public FormatSupport
{
	Q_OBJECT
public:
	explicit PaintFieldFormatSupport(QObject *parent = 0);
	
	QString name() const override;
	QStringList suffixes() const override;
	
	bool canRead() const override;
	bool canWrite() const override;
	
	Capabilities capabilities() const override;
	bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) override;
	bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option) override;
	
signals:
	
public slots:
	
};

} // namespace PaintField
