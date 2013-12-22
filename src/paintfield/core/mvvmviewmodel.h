#pragma once

#include <QObject>
#include "property.h"

namespace PaintField {

class MVVMViewModel : public QObject
{
	Q_OBJECT
public:
	explicit MVVMViewModel(QObject *parent = 0);
	~MVVMViewModel();

	QHash<QByteArray, SP<Property>> routeHash();

protected:
	void route(QObject *object, const QByteArray &propertyName, const QByteArray &routeName);
	void route(const SP<Property> &property, const QByteArray &routeName);

private:
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
