#pragma once

#include <QWidget>
#include "property.h"

namespace PaintField {

class MVVMViewModel;

class MVVMView : public QWidget
{
	Q_OBJECT
public:
	explicit MVVMView(QWidget *parent = 0);
	~MVVMView();

	/**
	 * Sets a view model.
	 * The ownership of the view model is transferred to this.
	 */
	void setViewModel(MVVMViewModel *viewModel);
	QHash<QByteArray, SP<Property>> routeHash();

protected:
	void route(QObject *object, const QByteArray &propertyName, const QByteArray &routeName);
	void route(const SP<Property> &property, const QByteArray &routeName);

private:
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
