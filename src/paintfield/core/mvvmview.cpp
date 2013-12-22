#include "mvvmview.h"
#include "mvvmviewmodel.h"

namespace PaintField {

struct MVVMView::Data
{
	QHash<QByteArray, SP<Property>> mRouteHash;
};

MVVMView::MVVMView(QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
}

MVVMView::~MVVMView()
{
}

void MVVMView::setViewModel(MVVMViewModel *viewModel)
{
	viewModel->setParent(viewModel);

	auto vmRouteHash = viewModel->routeHash();
	auto begin = d->mRouteHash.begin();
	auto end = d->mRouteHash.end();
	for (auto i = begin; i != end; ++i) {
		if (vmRouteHash.contains(i.key())) {
			Property::sync(i.value(), vmRouteHash.value(i.key()));
		}
	}
}

void MVVMView::route(const SP<Property> &property, const QByteArray &routeName)
{
	d->mRouteHash[routeName] = property;
}

void MVVMView::route(QObject *object, const QByteArray &propertyName, const QByteArray &routeName)
{
	d->mRouteHash[routeName] = qtProperty(object, propertyName);
}

QHash<QByteArray, SP<Property>> MVVMView::routeHash()
{
	return d->mRouteHash;
}

} // namespace PaintField
