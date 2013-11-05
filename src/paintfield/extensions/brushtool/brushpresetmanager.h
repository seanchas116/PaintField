#pragma once

#include <QObject>
#include <QStringList>
#include <QVariant>

namespace PaintField {

class BrushPresetItem;
class ObservableVariantMap;

class BrushPresetManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushPresetManager(QObject *parent = 0);
	~BrushPresetManager();

	QString title() const;
	void setTitle(const QString &title);

	QString stroker() const;
	void setStroker(const QString &stroker);

	ObservableVariantMap *parameters();
	ObservableVariantMap *commonParameters();

public slots:
	
	void setPreset(BrushPresetItem *item);
	
signals:
	
	void titleChanged(const QString &title);
	void strokerChanged(const QString &stroker);
	
private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

