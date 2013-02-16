#ifndef CALLBACKANIMATION_H
#define CALLBACKANIMATION_H

#include <functional>
#include <QVariantAnimation>

namespace PaintField
{

class CallbackAnimation : public QVariantAnimation
{
	Q_OBJECT
public:
	explicit CallbackAnimation(QObject *parent = 0);
	
	void setCallback(const std::function<void(QVariant)> &callback) { _callback = callback; }
	
	template <typename T>
	void setCallbackForType(const std::function<void(T)> &callback)
	{
		auto newCallback = [=](const QVariant &variant)
		{
			callback(variant.value<T>());
		};
		
		setCallback(newCallback);
	}
	
signals:
	
public slots:
	
protected:
	
	void updateCurrentValue(const QVariant &value);
	
private:
	
	std::function<void(QVariant)> _callback;
};

}

#endif // CALLBACKANIMATION_H
