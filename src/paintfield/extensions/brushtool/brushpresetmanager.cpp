#include "brushpresetmanager.h"

#include "brushpresetitem.h"
#include "paintfield/core/json.h"
#include "paintfield/core/observablevariantmap.h"

namespace PaintField {

struct BrushPresetManager::Data
{
	ObservableVariantMap *mParameters;
	ObservableVariantMap *mCommonParameters;
	BrushPresetItem *mPresetItem = nullptr;
};

BrushPresetManager::BrushPresetManager(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->mParameters = new ObservableVariantMap(this);
	d->mCommonParameters = new ObservableVariantMap(this);
	connect(d->mParameters, &ObservableVariantMap::mapChanged, [this](const QVariantMap &map) {
		if (d->mPresetItem)
			d->mPresetItem->parametersRef() = map;
	});
}

BrushPresetManager::~BrushPresetManager()
{

}

QString BrushPresetManager::title() const
{
	if (d->mPresetItem)
		return d->mPresetItem->text();
	else
		return QString();
}

void BrushPresetManager::setTitle(const QString &title)
{
	if (d->mPresetItem && title != d->mPresetItem->text()) {
		d->mPresetItem->setText(title);
		emit titleChanged(title);
	}
}

QString BrushPresetManager::stroker() const
{
	if (d->mPresetItem)
		return d->mPresetItem->stroker();
	else
		return QString();
}

void BrushPresetManager::setStroker(const QString &stroker)
{
	if (d->mPresetItem && stroker != d->mPresetItem->stroker()) {
		d->mPresetItem->setStroker(stroker);
		emit strokerChanged(stroker);
	}
}

ObservableVariantMap *BrushPresetManager::parameters()
{
	return d->mParameters;
}

ObservableVariantMap *BrushPresetManager::commonParameters()
{
	return d->mCommonParameters;
}

void BrushPresetManager::setPreset(BrushPresetItem *item)
{
	d->mPresetItem = item;
	if (item) {
		emit titleChanged(item->text());
		emit strokerChanged(item->stroker());
		d->mParameters->setMap(item->parameters());
	} else {
		emit titleChanged(QString());
		emit strokerChanged(QString());
		d->mParameters->setMap(QVariantMap());
	}

}

} // namespace PaintField
