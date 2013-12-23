#pragma once
#include "global.h"
#include <QObject>
#include <QMetaProperty>
#include <array>

namespace PaintField {

namespace PropertyDetail {

template <class T>
struct UnaryMemberFunctionArgument;

template <class T, class A, class R>
struct UnaryMemberFunctionArgument<R (T::*)(A) const>
{
	using type = A;
};

template <class T>
using RemoveConstReferenceType = typename std::remove_const<
	typename std::remove_reference<T>::type
>::type;

template <class F>
using ArgumentType = RemoveConstReferenceType<
	typename UnaryMemberFunctionArgument<decltype(&F::operator())>::type
>;

template <class F>
class VariantTransformWrapper
{
public:
	VariantTransformWrapper(const F &f) : mF(f) {}

	QVariant operator()(const QVariant &x) const
	{
		return QVariant::fromValue(mF(x.value<ArgumentType<F>>()));
	}

private:
	F mF;
};

template <class F>
class VariantSetterWrapper
{
public:
	VariantSetterWrapper(const F &f) : mF(f) {}

	void operator()(const QVariant &x) const
	{
		mF(x.value<ArgumentType<F>>());
	}

private:
	F mF;
};

template <class F>
class VariantGetterWrapper
{
public:
	VariantGetterWrapper(const F &f) : mF(f) {}

	QVariant operator()() const
	{
		return QVariant::fromValue(mF());
	}

private:
	F mF;
};

using Transform = std::function<QVariant (const QVariant &)>;
using Setter = std::function<void (const QVariant &)>;
using Getter = std::function<QVariant ()>;

template <class F>
Transform toVariantTransform(const F &f)
{
	return VariantTransformWrapper<F>(f);
}

template <class F>
Setter toVariantSetter(const F &f)
{
	return VariantSetterWrapper<F>(f);
}

template <class F>
Getter toVariantGetter(const F &f)
{
	return VariantGetterWrapper<F>(f);
}


} // namespace PropertyDetail


class Property
{
public:

	class Connection
	{
		friend class Property;
	public:
		bool isValid() const { return mObject; }
		explicit operator bool() const { return isValid(); }
		void disconnect();

	private:
		explicit Connection(QObject *object);
		QObject *mObject;
	};

	/**
	 * Binds the first property with the second property.
	 * @param p2 Must not be read-only
	 */
	static Connection bind(const SP<Property> &p1, const SP<Property> &p2);
	static Connection bind(QObject *object1, const QByteArray &propertyName1, QObject *object2, const QByteArray &propertyName2);

	template <class F1>
	static Connection bind(
		QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
		QObject *object2, const QByteArray &propertyName2);

	template <class F1>
	static Connection bind(const SP<Property> &p1, const F1 &transformTo1, const SP<Property> &p2)
	{
		return bind(p1, PropertyDetail::toVariantTransform(transformTo1), p2);
	}

	static Connection bind(const SP<Property> &p1, const PropertyDetail::Transform &transformTo1, const SP<Property> &p2);

	/**
	 * Binds the two properties bidirectionally.
	 * Both properties are initialized with the second one when this function is called.
	 */
	static Connection sync(const SP<Property> &p1, const SP<Property> &p2);
	static Connection sync(QObject *object1, const QByteArray &propertyName1, QObject *object2, const QByteArray &propertyName2);

	/**
	 * Binds the two properties with two unary transform functions.
	 * Their argument types must be convertible from QVariant and return types must be convertible to QVariant.
	 */
	template <class F1, class F2>
	static Connection sync(const SP<Property> &p1, const F1 &transformTo1, const SP<Property> &p2, const F2 &transformTo2)
	{
		return sync(
			p1, PropertyDetail::toVariantTransform(transformTo1),
			p2, PropertyDetail::toVariantTransform(transformTo2));
	}

	template <class F1, class F2>
	static Connection sync(
		QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
		QObject *object2, const QByteArray &propertyName2, const F2 &transformTo2);

	static Connection sync(
		const SP<Property> &p1, const PropertyDetail::Transform &transformTo1,
		const SP<Property> &p2, const PropertyDetail::Transform &transformTo2);

	Property(const Property &) = delete;
	Property& operator=(const Property &) = delete;

	virtual void set(const QVariant &value) = 0;
	virtual QVariant get() const = 0;
	QObject *object() const { return mObject; }
	QMetaMethod notifySignal() const { return mNotifySignal; }

	bool isNotifiable() const;
	virtual bool hasSetter() const = 0;
	virtual bool hasGetter() const = 0;

protected:
	void setNotify(QObject *object, const QMetaMethod &signal)
	{
		mObject = object;
		mNotifySignal = signal;
	}

	Property() = default;

private:
	QObject *mObject = 0;
	QMetaMethod mNotifySignal;
};


class QtProperty : public Property
{
public:
	QtProperty(QObject *object, const QByteArray &propertyName);

	void set(const QVariant &value) override
	{
		mProperty.write(object(), value);
	}
	QVariant get() const override
	{
		return mProperty.read(object());
	}
	bool hasSetter() const override
	{
		return mProperty.isWritable();
	}
	bool hasGetter() const override
	{
		return mProperty.isReadable();
	}

private:
	QMetaProperty mProperty;
};

inline SP<Property> qtProperty(QObject *object, const QByteArray &propertyName)
{
	return makeSP<QtProperty>(object, propertyName);
}

class CustomProperty : public Property
{
public:

	CustomProperty(
		const PropertyDetail::Setter &setter, const PropertyDetail::Getter &getter,
		QObject *object, const QMetaMethod &notifySignal);

	void set(const QVariant &value) override
	{
		mSetter(value);
	}
	QVariant get() const override
	{
		return mGetter();
	}
	bool hasSetter() const override
	{
		return bool(mSetter);
	}
	bool hasGetter() const override
	{
		return bool(mGetter);
	}

private:
	PropertyDetail::Setter mSetter;
	PropertyDetail::Getter mGetter;
};

template <class F1, class F2>
inline SP<Property> customProperty(
	QObject *object, const F1 &setter, const F2 &getter,
	const QMetaMethod &notifySignal)
{
	return makeSP<CustomProperty>(
		PropertyDetail::toVariantSetter(setter), PropertyDetail::toVariantGetter(getter),
		object, notifySignal);
}

template <class F1, class F2>
inline SP<Property> customProperty(
	QObject *object, const F1 &setter, const F2 &getter,
	const char *notifySignal)
{
	auto metaObject = object->metaObject();
	return customProperty(object, setter, getter,
		metaObject->method(metaObject->indexOfSignal(notifySignal + 1)));
}

template <class F>
inline SP<Property> customProperty(
	QObject *object, const F &getter, const QMetaMethod &notifySignal)
{
	return customProperty(object, PropertyDetail::Setter(), getter, notifySignal);
}

template <class F>
inline SP<Property> customProperty(
	QObject *object, const F &getter, const char *notifySignal)
{
	return customProperty(object, PropertyDetail::Setter(), getter, notifySignal);
}

template <class F>
inline SP<Property> customProperty(QObject *object, const F &setter)
{
	return customProperty(object, setter, PropertyDetail::Getter(), QMetaMethod());
}


template <class F1>
Property::Connection Property::bind(
	QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
	QObject *object2, const QByteArray &propertyName2)
{
	return Property::bind(
		qtProperty(object1, propertyName1), transformTo1,
		qtProperty(object2, propertyName2));
}

template <class F1, class F2>
Property::Connection Property::sync(
	QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
	QObject *object2, const QByteArray &propertyName2, const F2 &transformTo2)
{
	return Property::sync(
		qtProperty(object1, propertyName1), transformTo1,
		qtProperty(object2, propertyName2), transformTo2);
}

} // namespace PaintField
