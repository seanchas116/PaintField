#include <QApplication>
#include <QKeySequence>
#include "keytracker.h"

namespace PaintField {

void KeyTracker::keyPressed(int key)
{
	if (!_keys.contains(key))
		_keys << key;
}

void KeyTracker::keyReleased(int key)
{
	_keys.removeAll(key);
}

bool KeyTracker::match(const QKeySequence &sequence) const
{
	auto keyToModifier = [](int key)
	{
		switch (key)
		{
			case Qt::Key_Shift:
				return Qt::ShiftModifier;
			case Qt::Key_Control:
				return Qt::ControlModifier;
			case Qt::Key_Meta:
				return Qt::MetaModifier;
			case Qt::Key_Alt:
				return Qt::AltModifier;
			default:
				return Qt::NoModifier;
		}
	};
	
	Qt::KeyboardModifiers modifiers = Qt::NoModifier;
	QList<int> normalKeys;
	
	for (int key : _keys)
	{
		auto modifier = keyToModifier(key);
		
		if (modifier == Qt::NoModifier)
			normalKeys << key;
		else
			modifiers |= modifier;
	}
	
	if (sequence.isEmpty())
		return false;
	
	if (normalKeys.size() != 1)
		return false;
	
	return normalKeys[0] + modifiers == sequence[0];
}

} // namespace PaintField
