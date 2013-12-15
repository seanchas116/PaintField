#pragma once
#include "global.h"
#include <QObject>
#include <QMetaProperty>
#include <array>

namespace PaintField {

namespace Property {

namespace detail {

class BindObject : public QObject
{
	Q_OBJECT
public:
	BindObject(QObject *obj1, const QByteArray &propertyName1, QObject *obj2, const QByteArray &propertyName2);

private slots:
	void on1Changed();
	void on2Changed();

private:
	QObject *mObj1, *mObj2;
	QMetaProperty mProperty1, mProperty2;
};

class BindTransformObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariant value1 READ value1 WRITE setValue1 NOTIFY value1Changed)
	Q_PROPERTY(QVariant value2 READ value2 WRITE setValue2 NOTIFY value2Changed)

	enum Channel { Channel1, Channel2 };

public:
	BindTransformObject(std::function<QVariant(const QVariant &)> to1, std::function<QVariant(const QVariant &)> to2);
	void setValue1(const QVariant &x) { setValue(x, Channel1); }
	void setValue2(const QVariant &x) { setValue(x, Channel2); }
	QVariant value1() const { return mValue[Channel1]; }
	QVariant value2() const { return mValue[Channel2]; }

signals:
	void value1Changed(const QVariant &);
	void value2Changed(const QVariant &);

private:
	static Channel opposite(Channel c) { return c == Channel1 ? Channel2 : Channel1; }
	void setValue(const QVariant &value, Channel ch);
	void emitValueChanged(Channel ch);

	std::array<QVariant, 2> mValue;
	std::array<std::function<QVariant(const QVariant &)>, 2> mTo;
};

template <class T>
struct FirstArgument;

template <class T, class A, class R>
struct FirstArgument<R (T::*)(A) const> {
	using type = A;
};

template <class PointerToMemberFunction>
using FirstArgumentType = typename FirstArgument<PointerToMemberFunction>::type;

template <class F>
class VariantFunctionWrapper
{
	using ArgumentType = FirstArgumentType<decltype(&F::operator())>;
public:
	VariantFunctionWrapper(F f) : mF(f) {}

	QVariant operator()(const QVariant &x) const
	{
		return mF(x.value<ArgumentType>());
	}

private:
	F mF;
};

template <class F>
VariantFunctionWrapper<F> makeVariantFunctionWrapper(F f)
{
	return VariantFunctionWrapper<F>(f);
}

} // namespace detail

void bind(QObject *obj1, const QByteArray &propertyName1, QObject *obj2, const QByteArray &propertyName2);

template <class F1, class F2>
void bind(QObject *obj1, const QByteArray &propertyName1, F1 transformTo1, QObject *obj2, const QByteArray &propertyName2, F2 transformTo2)
{
	auto t = new detail::BindTransformObject(detail::makeVariantFunctionWrapper(transformTo1), detail::makeVariantFunctionWrapper(transformTo2));
	auto b = new detail::BindObject(t, "value1", obj1, propertyName1);
	new detail::BindObject(t, "value2", obj2, propertyName2);
	t->setParent(b);
}

} // namespace Property

} // namespace PaintField
