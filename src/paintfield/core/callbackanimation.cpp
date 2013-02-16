#include "callbackanimation.h"

namespace PaintField
{

CallbackAnimation::CallbackAnimation(QObject *parent) :
	QVariantAnimation(parent)
{
}

void CallbackAnimation::updateCurrentValue(const QVariant &value)
{
	if (_callback)
		_callback(value);
}

}
