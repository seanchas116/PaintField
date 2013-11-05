#pragma once

#include <QObject>
#include <QVariant>
#include <type_traits>

namespace PaintField {

class ObservableVariantMap : public QObject
{
	Q_OBJECT
public:
	explicit ObservableVariantMap(QObject *parent = 0);

	QVariant value(const QString &key) const;
	QVariantMap map() const;

	template <class TFuncArg, class TObject, class TSignal>
	void setValueOn(const QString &key, TObject *obj, TSignal signal)
	{
		connect(obj, signal, this, [this, key](const TFuncArg &value) {
			setValue(key, value);
		});
	}

	template <class TFuncArg, class TFunc>
	typename std::enable_if<!std::is_member_function_pointer<TFunc>::value>::type
	onValueChanged(const QString &key, QObject *context, TFunc f)
	{
		connect(this, &ObservableVariantMap::valueChanged, context, [key, f](const QString &k, const QVariant &value){
			if (k == key)
				f(value.value<TFuncArg>());
		});
	}

	template <class TFuncArg, class TObject, class TFunc>
	typename std::enable_if<std::is_member_function_pointer<TFunc>::value>::type
	onValueChanged(const QString &key, TObject *object, TFunc f)
	{
		connect(this, &ObservableVariantMap::valueChanged, object, [object, key, f](const QString &k, const QVariant &value){
			if (k == key)
				(object->*f)(value.value<TFuncArg>());
		});
	}

signals:

	void mapChanged(const QVariantMap &map);
	void valueChanged(const QString &key, const QVariant &value);

public slots:

	void setValue(const QString &key, const QVariant &value);
	void setMap(const QVariantMap &map);

private:
	QVariantMap mMap;
};

} // namespace PaintField
