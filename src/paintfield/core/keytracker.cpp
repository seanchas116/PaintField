#include <QApplication>
#include <QSet>
#include <QKeySequence>
#include "keytracker.h"

namespace PaintField {

struct KeyTracker::Data
{
	QSet<int> keys;
	Qt::KeyboardModifiers modifiers;
};

static Qt::KeyboardModifier keyToModifier(int key)
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
}

KeyTracker::KeyTracker(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	
}

KeyTracker::~KeyTracker()
{
	delete d;
}

void KeyTracker::pressKey(int key)
{
	auto modifier = keyToModifier(key);
	if (modifier != Qt::NoModifier)
		d->modifiers |= modifier;
	else
		d->keys << key;
	
	emit pressedKeysChanged(pressedKeys());
}

void KeyTracker::releaseKey(int key)
{
	auto modifier = keyToModifier(key);
	if (modifier != Qt::NoModifier)
		d->modifiers &= ~modifier;
	else
		d->keys.remove(key);
	
	emit pressedKeysChanged(pressedKeys());
}

void KeyTracker::setModifiers(Qt::KeyboardModifiers modifiers)
{
	d->modifiers = modifiers;
}

QSet<int> KeyTracker::unmodifiedPressedKeys() const
{
	return d->keys;
}

QSet<int> KeyTracker::pressedKeys() const
{
	QSet<int> keys;
	keys.reserve(d->keys.size());
	
	for (int key : d->keys)
		keys << (key + d->modifiers);
	
	return keys;
}

Qt::KeyboardModifiers KeyTracker::modifiers() const
{
	return d->modifiers;
}

bool KeyTracker::match(const QKeySequence &sequence) const
{
	auto keys = pressedKeys();
	
	PAINTFIELD_DEBUG << keys;
	PAINTFIELD_DEBUG << sequence;
	
	return keys.size() == 1 && sequence.count() == 1 && *keys.begin() == sequence[0];
}

} // namespace PaintField
