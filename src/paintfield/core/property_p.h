#pragma once

#include "property.h"
#include <QObject>

namespace PaintField {

namespace PropertyDetail {

class BindObject : public QObject
{
	Q_OBJECT
public:
	BindObject(UP<Property> &&property1, UP<Property> &&property2);

private slots:
	void on1Changed()
	{
		mProperty2->set(mProperty1->get());
	}
	void on2Changed()
	{
		mProperty1->set(mProperty2->get());
	}

private:
	UP<Property> mProperty1, mProperty2;
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

}

}
