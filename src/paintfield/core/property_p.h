#pragma once

#include "property.h"
#include <QObject>

namespace PaintField {

namespace PropertyDetail {

class BindObject : public QObject
{
	Q_OBJECT
public:

	enum Mode { ModeSingly, ModeDoubly };
	enum Channel { Channel1, Channel2 };

	BindObject(const SP<Property> &property1, const SP<Property> &property2, Mode mode = ModeDoubly);

	static Channel opposite(Channel c) { return c == Channel1 ? Channel2 : Channel1; }
protected:
	const SP<Property> &property(Channel channel) { return mProperty[channel]; }

	virtual void onChanged(Channel channel)
	{
		mProperty[opposite(channel)]->set(mProperty[channel]->get());
	}

protected slots:
	void on1Changed() { onChanged(Channel1); }
	void on2Changed() { onChanged(Channel2); }

private:
	std::array<SP<Property>, 2> mProperty;
};

class BindTransformObject : public BindObject
{
	Q_OBJECT

public:
	BindTransformObject(const SP<Property> &property1, const SP<Property> &property2,
		const Transform &to1, const Transform &to2,
		Mode mode = ModeDoubly);

protected:
	void onChanged(Channel channel) override;

private:
	std::array<QVariant, 2> mValue;
	std::array<std::function<QVariant(const QVariant &)>, 2> mTo;
};

}

}
