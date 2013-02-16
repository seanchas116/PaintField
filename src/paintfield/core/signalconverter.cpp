
#include "signalconverter.h"

namespace PaintField {

SignalConverter::SignalConverter(VariantFunc funcAToB, VariantFunc funcBToA, QObject *parent) :
    QObject(parent),
    _funcAToB(funcAToB),
    _funcBToA(funcBToA)
{}

SignalConverter *SignalConverter::fromIntFunc(std::function<int (int)> funcAToB, std::function<int (int)> funcBToA, QObject *parent)
{
	return new SignalConverter([=](const QVariant &x){return funcAToB(x.toInt());}, [=](const QVariant &x){return funcBToA(x.toInt());}, parent);
}

SignalConverter *SignalConverter::fromDoubleFunc(std::function<double (double)> funcAToB, std::function<double (double)> funcBToA, QObject *parent)
{
	return new SignalConverter([=](const QVariant &x){return funcAToB(x.toDouble());}, [=](const QVariant &x){return funcBToA(x.toDouble());}, parent);
}

void SignalConverter::connectChannel(Channel channel, QObject *obj, const char *signal, const char *setter)
{
	const char *channelSignal;
	const char *channelSetter;
	if (channel == ChannelA)
	{
		channelSignal = SIGNAL(valueAChanged(QVariant));
		channelSetter = SLOT(setValueA(QVariant));
	}
	else
	{
		channelSignal = SIGNAL(valueBChanged(QVariant));
		channelSetter = SLOT(setValueB(QVariant));
	}
	
	connect(obj, signal, this, channelSetter);
	connect(this, channelSignal, obj, setter);
}

void SignalConverter::connectChannelInt(Channel channel, QObject *obj, const char *signal, const char *setter)
{
	const char *channelSignal;
	const char *channelSetter;
	if (channel == ChannelA)
	{
		channelSignal = SIGNAL(valueAChanged(int));
		channelSetter = SLOT(setValueA(int));
	}
	else
	{
		channelSignal = SIGNAL(valueBChanged(int));
		channelSetter = SLOT(setValueB(int));
	}
	
	connect(obj, signal, this, channelSetter);
	connect(this, channelSignal, obj, setter);
}

void SignalConverter::connectChannelDouble(Channel channel, QObject *obj, const char *signal, const char *setter)
{
	const char *channelSignal;
	const char *channelSetter;
	if (channel == ChannelA)
	{
		channelSignal = SIGNAL(valueAChanged(double));
		channelSetter = SLOT(setValueA(double));
	}
	else
	{
		channelSignal = SIGNAL(valueBChanged(double));
		channelSetter = SLOT(setValueB(double));
	}
	
	connect(obj, signal, this, channelSetter);
	connect(this, channelSignal, obj, setter);
}

void SignalConverter::emitValueChanged(const QVariant &value, Channel channel)
{
	if (channel == ChannelA)
	{
		emit valueAChanged(value);
		emit valueAChanged(value.toInt());
		emit valueAChanged(value.toDouble());
	}
	else
	{
		emit valueBChanged(value);
		emit valueBChanged(value.toInt());
		emit valueBChanged(value.toDouble());
	}
}

void SignalConverter::setValue(const QVariant &value, Channel channel)
{
	Channel other = otherChannel(channel);
	
	if (rvalue(channel) != value)
	{
		rvalue(channel) = value;
		emitValueChanged(value, channel);
		
		rvalue(other) = applyFunc(channel, value);
		emitValueChanged(rvalue(other), other);
	}
}

} // namespace PaintField
