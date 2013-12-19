#include "property.h"
#include "property_p.h"
#include <QMetaProperty>

namespace PaintField {

namespace PropertyDetail {

BindObject::BindObject(UP<Property> &&p1, UP<Property> &&p2) :
	mProperty1(std::move(p1)),
	mProperty2(std::move(p2))
{
	auto getSlot = [](const char *name) {
		// SLOT macro prepends "1" to method name
		return staticMetaObject.method(staticMetaObject.indexOfSlot(name + 1));
	};
	if (mProperty1->notifyObject()) {
		connect(mProperty1->notifyObject(), mProperty1->notifySignal(), this, getSlot(SLOT(on1Changed())));
	}
	if (mProperty2->notifyObject()) {
		connect(mProperty2->notifyObject(), mProperty2->notifySignal(), this, getSlot(SLOT(on2Changed())));
	}
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

} // namespace PropertyDetail

void Property::bind(UP<Property> &&p1, UP<Property> &&p2)
{
	new PropertyDetail::BindObject(std::move(p1), std::move(p2));
}

void Property::bind(QObject *object1, const QByteArray &propertyName1, QObject *object2, const QByteArray &propertyName2)
{
	new PropertyDetail::BindObject(qtProperty(object1, propertyName1), qtProperty(object2, propertyName2));
}

void Property::bind(UP<Property> &&p1, const std::function<QVariant (const QVariant &)> &transformTo1, UP<Property> &&p2, const std::function<QVariant (const QVariant &)> &transformTo2)
{
	auto t = new PropertyDetail::BindTransformObject(transformTo1, transformTo2);
	auto b = new PropertyDetail::BindObject(qtProperty(t, "value1"), std::move(p1));
	new PropertyDetail::BindObject(qtProperty(t, "value2"), std::move(p2));
	t->setParent(b);
}

QtProperty::QtProperty(QObject *object, const QByteArray &propertyName)
{
	auto m = object->metaObject();
	mProperty = m->property(m->indexOfProperty(propertyName.data()));
	setNotify(object, mProperty.notifySignal());
}

CustomProperty::CustomProperty(
	std::function<void (const QVariant &)> setter, std::function<QVariant ()> getter,
	QObject *object, const QMetaMethod &notifySignal) :
	mSetter(setter),
	mGetter(getter)
{
	setNotify(object, notifySignal);
}

} // namespace PaintField
