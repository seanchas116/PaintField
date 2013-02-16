#ifndef PAINTFIELD_SIGNALCHANGER_H
#define PAINTFIELD_SIGNALCHANGER_H

#include <QObject>
#include <QVariant>
#include <functional>

namespace PaintField {

class SignalConverter : public QObject
{
	Q_OBJECT
public:
	
	typedef std::function<QVariant(const QVariant &)> VariantFunc;
	
	enum Channel
	{
		ChannelA,
		ChannelB
	};
	
	SignalConverter(VariantFunc funcAToB, VariantFunc funcBToA,  QObject *parent);
	
	static SignalConverter *fromIntFunc(std::function<int(int)> funcAToB, std::function<int(int)> funcBToA, QObject *parent);
	static SignalConverter *fromDoubleFunc(std::function<double(double)> funcAToB, std::function<double(double)> funcBToA, QObject *parent);
	
	static Channel otherChannel(Channel ch) { return ch == ChannelA ? ChannelB : ChannelA; }
	
	void connectChannel(Channel channel, QObject *obj, const char *signal, const char *setter);
	void connectChannelInt(Channel channel, QObject *obj, const char *signal, const char *setter);
	void connectChannelDouble(Channel channel, QObject *obj, const char *signal, const char *setter);
	
	void connectChannelA(QObject *obj, const char *signal, const char *setter) { connectChannel(ChannelA, obj, signal, setter); }
	void connectChannelB(QObject *obj, const char *signal, const char *setter) { connectChannel(ChannelB, obj, signal, setter); }
	void connectChannelAInt(QObject *obj, const char *signal, const char *setter) { connectChannelInt(ChannelA, obj, signal, setter); }
	void connectChannelBInt(QObject *obj, const char *signal, const char *setter) { connectChannelInt(ChannelB, obj, signal, setter); }
	void connectChannelADouble(QObject *obj, const char *signal, const char *setter) { connectChannelDouble(ChannelA, obj, signal, setter); }
	void connectChannelBDouble(QObject *obj, const char *signal, const char *setter) { connectChannelDouble(ChannelB, obj, signal, setter); }
	
signals:
	
	void valueAChanged(const QVariant &value);
	void valueAChanged(int value);
	void valueAChanged(double value);
	
	void valueBChanged(const QVariant &value);
	void valueBChanged(int value);
	void valueBChanged(double value);
	
public slots:
	
	void setValueA(int value) { setValueA(QVariant(value)); }
	void setValueA(double value) { setValueA(QVariant(value)); }
	void setValueA(const QVariant &value) { setValue(value, ChannelA); }
	
	void setValueB(int value) { setValueB(QVariant(value)); }
	void setValueB(double value) { setValueB(QVariant(value)); }
	void setValueB(const QVariant &value) { setValue(value, ChannelB); }
	
private:
	
	QVariant &rvalue(Channel channel)
	{
		if (channel == ChannelA)
			return _valueA;
		else
			return _valueB;
	}
	
	QVariant applyFunc(Channel channelFrom, const QVariant &value)
	{
		if (channelFrom == ChannelA)
			return _funcAToB(value);
		else
			return _funcBToA(value);
	}
	
	void emitValueChanged(const QVariant &value, Channel channel);
	void setValue(const QVariant &value, Channel channel);
	
	VariantFunc _funcAToB, _funcBToA;
	QVariant _valueA, _valueB;
};


} // namespace PaintField

#endif // PAINTFIELD_SIGNALCHANGER_H
