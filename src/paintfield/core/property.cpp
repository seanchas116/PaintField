#include "property.h"
#include <QMetaProperty>

namespace PaintField {

namespace Property {

namespace detail {

BindObject::BindObject(QObject *obj1, const QByteArray &propertyName1, QObject *obj2, const QByteArray &propertyName2) :
	mObj1(obj1),
	mObj2(obj2)
{
	auto getMetaProperty = [](QObject *obj, const QByteArray &name) {
		return obj->metaObject()->property(obj->metaObject()->indexOfProperty(name.data()));
	};
	auto getMethod = [](QObject *obj, const char *name) {
		return obj->metaObject()->method(obj->metaObject()->indexOfMethod(name));
	};

	mProperty1 = getMetaProperty(obj1, propertyName1);
	mProperty2 = getMetaProperty(obj2, propertyName2);
	connect(obj1, mProperty1.notifySignal(), this, getMethod(this, "on1Changed()"));
	connect(obj2, mProperty2.notifySignal(), this, getMethod(this, "on2Changed()"));
	on2Changed();
	connect(obj1, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(obj2, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


void BindObject::on1Changed()
{
	auto value = mProperty1.read(mObj1);
	mProperty2.write(mObj2, value);
}

void BindObject::on2Changed()
{
	auto value = mProperty2.read(mObj2);
	mProperty1.write(mObj1, value);
}

BindTransformObject::BindTransformObject(
	std::function<QVariant (const QVariant &)> to1, std::function<QVariant (const QVariant &)> to2)
{
	mTo[Channel1] = to1;
	mTo[Channel2] = to2;
}

void BindTransformObject::setValue(const QVariant &value, Channel ch)
{
	auto other = opposite(ch);
	if (mValue[ch] != value) {
		mValue[ch] = value;
		mValue[other] = mTo[other](value);
		emitValueChanged(ch);
		emitValueChanged(other);
	}
}

void BindTransformObject::emitValueChanged(Channel ch)
{
	if (ch == Channel1)
		emit value1Changed(mValue[ch]);
	else
		emit value2Changed(mValue[ch]);
}

} // namespace detail

void bind(QObject *obj1, const QByteArray &propertyName1, QObject *obj2, const QByteArray &propertyName2)
{
	new detail::BindObject(obj1, propertyName1, obj2, propertyName2);
}

} // namespace Property

} // namespace PaintField
