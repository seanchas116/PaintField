#pragma once

#include <QObject>
#include "layer.h"

namespace PaintField {

class FormatSupport : public QObject
{
	Q_OBJECT
public:
	explicit FormatSupport(QObject *parent = 0);
	~FormatSupport();
	
	enum class Capability
	{
		Layers,
		AlphaChannel,
		Lossless
	};
	
	virtual QString name() const = 0;
	virtual QStringList suffixes() const = 0;
	QString shortDescription() const;
	QString longDescription() const;
	
	virtual bool canRead() const;
	virtual bool canWrite() const;
	
	virtual bool hasCapability(Capability capability) const = 0;
	virtual bool read(QIODevice *device, QList<LayerRef> *layers, QSize *size) = 0;
	virtual bool write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size) = 0;
	
	virtual QWidget *createExportOptionWidget();
	virtual void setExportOptions(QWidget *widget);
	
	static bool importFromFileDialog(const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, const QString &dialogTitle);
	static bool exportToFileDialog(const QList<FormatSupport *> &formatSupports, const QList<LayerConstRef> &layers, const QSize &size, const QString &dialogTitle, bool showOptions);
	
signals:
	
public slots:
	
protected:
	
	void setShortDescription(const QString &text);
	void setLongDescription(const QString &text);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
