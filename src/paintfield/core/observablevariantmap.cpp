#include "observablevariantmap.h"

namespace PaintField {

ObservableVariantMap::ObservableVariantMap(QObject *parent) :
    QObject(parent)
{
}

QVariant ObservableVariantMap::value(const QString &key) const
{
	return mMap[key];
}

QVariantMap ObservableVariantMap::map() const
{
	return mMap;
}

void ObservableVariantMap::setValue(const QString &key, const QVariant &value)
{
	if (mMap[key] != value) {
		mMap[key] = value;
		emit valueChanged(key, value);
		emit mapChanged(mMap);
	}
}

void ObservableVariantMap::setMap(const QVariantMap &map)
{
	if (mMap == map)
		return;

	mMap = map;
	auto i = map.begin();
	auto end = map.end();
	for (; i != end; ++i) {
		emit valueChanged(i.key(), i.value());
	}
	emit mapChanged(mMap);
}

} // namespace PaintField
