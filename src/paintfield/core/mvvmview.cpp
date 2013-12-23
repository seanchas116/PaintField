#include "mvvmview.h"
#include "mvvmviewmodel.h"

namespace PaintField {

struct MVVMView::Data
{
	QHash<QByteArray, SP<Property>> mRouteHash;
	MVVMViewModel *mViewModel = nullptr;
	QList<Property::Connection> mConnections;
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
	if (d->mViewModel == viewModel)
		return;

	for (auto &connection : d->mConnections) {
		connection.disconnect();
	}
	d->mConnections.clear();

	d->mViewModel = viewModel;
	viewModel->setParent(this);

	auto vmRouteHash = viewModel->routeHash();
	auto begin = d->mRouteHash.begin();
	auto end = d->mRouteHash.end();
	for (auto i = begin; i != end; ++i) {
		if (vmRouteHash.contains(i.key())) {
			auto connection = Property::sync(i.value(), vmRouteHash.value(i.key()));
			d->mConnections << connection;
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
