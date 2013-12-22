#include "mvvmviewmodel.h"

namespace PaintField {

struct MVVMViewModel::Data
{
	QHash<QByteArray, SP<Property>> mRouteHash;
};

MVVMViewModel::MVVMViewModel(QObject *parent) :
	QObject(parent),
	d(new Data)
{
}

MVVMViewModel::~MVVMViewModel()
{
}

void MVVMViewModel::route(const SP<Property> &property, const QByteArray &routeName)
{
	d->mRouteHash[routeName] = property;
}

void MVVMViewModel::route(QObject *object, const QByteArray &propertyName, const QByteArray &routeName)
{
	this->route(qtProperty(object, propertyName), routeName);
}

QHash<QByteArray, SP<Property>> MVVMViewModel::routeHash()
{
	return d->mRouteHash;
}

} // namespace PaintField
