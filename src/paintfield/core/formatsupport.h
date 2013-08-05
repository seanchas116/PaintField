#pragma once

#include <QObject>
#include "layer.h"

namespace PaintField {

class Document;

class FormatSupport : public QObject
{
	Q_OBJECT
public:
	
	enum Capability
	{
		CapabilityLayers = 1 << 1,
		CapabilityAlphaChannel = 1 << 2,
		CapabilityLossless = 1 << 3,
		CapabilityAll = CapabilityLayers | CapabilityAlphaChannel | CapabilityLossless
	};
	Q_DECLARE_FLAGS(Capabilities, Capability)
	
	static bool importFromFile(const QString &filepath, const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, QString *name = 0);
	static bool importFromFileDialog(QWidget *parent, const QString &dialogTitle, const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, QString *name = 0, QString *path = 0);
	static bool exportToFile(const QString &filepath, FormatSupport *formatSupport, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option = QVariant());
	static bool exportToFileDialog(QWidget *parent, const QString &dialogTitle, bool showOptions, const QList<FormatSupport *> &formatSupports, const QList<LayerConstRef> &layers, const QSize &size);
	
	static QList<Capability> allCapabilities()
	{
		return
		{
			CapabilityLayers,
			CapabilityAlphaChannel,
			CapabilityLossless
		};
	}
	
	explicit FormatSupport(QObject *parent = 0);
	~FormatSupport();
	
	virtual QString name() const = 0;
	virtual QStringList suffixes() const = 0;
	QString shortDescription() const;
	QString longDescription() const;
	
	virtual bool canRead() const;
	virtual bool canWrite() const;
	
	virtual Capabilities capabilities() const = 0;
	virtual bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) = 0;
	virtual bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option) = 0;
	
	virtual QWidget *createExportingOptionWidget();
	virtual QVariant exportingOptionForWidget(QWidget *widget);
	
signals:
	
public slots:
	
protected:
	
	void setShortDescription(const QString &text);
	void setLongDescription(const QString &text);
	
private:
	
	struct Data;
	Data *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FormatSupport::Capabilities)

} // namespace PaintField
