#include <QApplication>
#include <QSet>
#include <QKeySequence>
#include <QKeyEvent>

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

void KeyTracker::keyEvent(QKeyEvent *event)
{
	switch (event->type())
	{
		case QEvent::KeyPress:
			
			pressKey(event->key());
			setModifiers(event->modifiers());
			break;
			
		case QEvent::KeyRelease:
			
			releaseKey(event->key());
			break;
			
		default:
			
			break;
	}
}

void KeyTracker::pressKey(int key)
{
	auto modifier = keyToModifier(key);
	if (modifier != Qt::NoModifier)
	{
		if (d->modifiers & modifier)
			return;
		d->modifiers |= modifier;
	}
	else
	{
		if (d->keys.contains(key))
			return;
		d->keys << key;
	}
	
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
	
	return keys.size() == 1 && sequence.count() == 1 && *keys.begin() == sequence[0];
}

void KeyTracker::clear()
{
	d->keys.clear();
	d->modifiers = Qt::NoModifier;
}

} // namespace PaintField
