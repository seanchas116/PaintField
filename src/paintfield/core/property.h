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
class VariantFunctionWrapper
{
public:
	VariantFunctionWrapper(const F &f) : mF(f) {}

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


template <class F>
std::function<QVariant (const QVariant &)> toVariantFunction(const F &f)
{
	return VariantFunctionWrapper<F>(f);
}

template <class F>
std::function<void (const QVariant &)> toVariantSetter(const F &f)
{
	return VariantSetterWrapper<F>(f);
}

template <class F>
std::function<QVariant ()> toVariantGetter(const F &f)
{
	return VariantGetterWrapper<F>(f);
}


} // namespace PropertyDetail

class Property
{
public:

	/**
	 * Binds the two properties bidirectionally.
	 * Both properties are initialized with the second one when this function is called.
	 * @return A QObject that is used to bind properties
	 */
	static void bind(UP<Property> &&p1, UP<Property> &&p2);
	static void bind(QObject *object1, const QByteArray &propertyName1, QObject *object2, const QByteArray &propertyName2);

	/**
	 * Binds the two properties with two unary transform functions.
	 * Their argument types must be convertible from QVariant and return types must be convertible to QVariant.
	 */
	template <class F1, class F2>
	static void bind(UP<Property> &&p1, const F1 &transformTo1, UP<Property> &&p2, const F2 &transformTo2)
	{
		bind(
			std::move(p1), PropertyDetail::toVariantFunction(transformTo1),
			std::move(p2), PropertyDetail::toVariantFunction(transformTo2));
	}

	template <class F1, class F2>
	static void bind(
		QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
		QObject *object2, const QByteArray &propertyName2, const F2 &transformTo2);

	static void bind(
		UP<Property> &&p1, const std::function<QVariant (const QVariant &)> &transformTo1,
		UP<Property> &&p2, const std::function<QVariant (const QVariant &)> &transformTo2);

	Property(const Property &) = delete;
	Property& operator=(const Property &) = delete;

	virtual void set(const QVariant &value) = 0;
	virtual QVariant get() const = 0;
	QObject *notifyObject() const { return mNotifyObject; }
	QMetaMethod notifySignal() const { return mNotifySignal; }

protected:
	void setNotify(QObject *object, const QMetaMethod &signal)
	{
		mNotifyObject = object;
		mNotifySignal = signal;
	}

	Property() = default;

private:
	QObject *mNotifyObject = 0;
	QMetaMethod mNotifySignal;
};

class QtProperty : public Property
{
public:
	QtProperty(QObject *object, const QByteArray &propertyName);

	void set(const QVariant &value) override
	{
		mProperty.write(notifyObject(), value);
	}
	QVariant get() const override
	{
		return mProperty.read(notifyObject());
	}

private:
	QMetaProperty mProperty;
};

inline UP<Property> qtProperty(QObject *object, const QByteArray &propertyName)
{
	return makeUP<QtProperty>(object, propertyName);
}

class CustomProperty : public Property
{
public:
	CustomProperty(
		std::function<void(const QVariant &)> setter, std::function<QVariant()> getter,
		QObject *object, const QMetaMethod &notifySignal);

	void set(const QVariant &value) override
	{
		mSetter(value);
	}

	QVariant get() const override
	{
		return mGetter();
	}

private:
	std::function<void(const QVariant &)> mSetter;
	std::function<QVariant()> mGetter;
};

template <class F1, class F2>
inline UP<Property> customProperty(
	const F1 &setter, const F2 &getter,
	QObject *object, const QMetaMethod &notifySignal
)
{
	return makeUP<CustomProperty>(
		PropertyDetail::toVariantSetter(setter), PropertyDetail::toVariantGetter(getter),
		object, notifySignal);
}

template <class F1, class F2>
inline UP<Property> customProperty(
	const F1 &setter, const F2 &getter,
	QObject *object, const char *notifySignal
)
{
	auto metaObject = object->metaObject();
	return customProperty(setter, getter, object,
		metaObject->method(metaObject->indexOfSignal(notifySignal + 1)));
}

template <class F1, class F2>
void Property::bind(
	QObject *object1, const QByteArray &propertyName1, const F1 &transformTo1,
	QObject *object2, const QByteArray &propertyName2, const F2 &transformTo2)
{
	Property::bind(
		qtProperty(object1, propertyName1), transformTo1,
		qtProperty(object2, propertyName2), transformTo2);
}

} // namespace PaintField
