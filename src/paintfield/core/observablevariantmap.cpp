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
	auto &ref = mMap[key];
	if (ref != value) {
		ref = value;
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

SP<Property> ObservableVariantMap::customProperty(const QString &key)
{
	auto setter = [=](const QVariant &x) {
		this->setValue(key, x);
	};
	auto getter = [=]() {
		return this->value(key);
	};
	return PaintField::customProperty(setter, getter, this, SIGNAL(valueChanged(QString,QVariant)));
}

} // namespace PaintField
