#include <QApplication>
#include <QSet>
#include <QKeySequence>
#include <QKeyEvent>

#include "keytracker.h"

namespace PaintField {

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

struct KeyTracker::Data
{
	KeyTracker *mSelf;

	QSet<int> mKeys;
	Qt::KeyboardModifiers mModifiers;

	void pressKey(int key)
	{
		auto modifier = keyToModifier(key);
		if (modifier != Qt::NoModifier)
		{
			if (mModifiers & modifier)
				return;
			mModifiers |= modifier;
		}
		else
		{
			if (mKeys.contains(key))
				return;
			mKeys << key;
		}

		emit mSelf->pressedKeysChanged(mSelf->pressedKeys());
	}

	void releaseKey(int key)
	{
		auto modifier = keyToModifier(key);
		if (modifier != Qt::NoModifier)
			mModifiers &= ~modifier;
		else
			mKeys.remove(key);

		emit mSelf->pressedKeysChanged(mSelf->pressedKeys());
	}

	void keyEvent(QKeyEvent *event)
	{
		switch (event->type()) {
			case QEvent::KeyPress:
				pressKey(event->key());
				mModifiers = event->modifiers();
				break;
			case QEvent::KeyRelease:
				releaseKey(event->key());
				break;
			default:
				break;
		}
	}
};

KeyTracker::KeyTracker(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->mSelf = this;
}

KeyTracker::~KeyTracker()
{
}

QSet<int> KeyTracker::unmodifiedPressedKeys() const
{
	return d->mKeys;
}

QSet<int> KeyTracker::pressedKeys() const
{
	QSet<int> keys;
	keys.reserve(d->mKeys.size());
	
	for (int key : d->mKeys)
		keys << (key + d->mModifiers);
	
	return keys;
}

Qt::KeyboardModifiers KeyTracker::modifiers() const
{
	return d->mModifiers;
}

bool KeyTracker::matches(const QKeySequence &sequence) const
{
	auto keys = pressedKeys();
	
	return keys.size() == 1 && sequence.count() == 1 && *keys.begin() == sequence[0];
}

void KeyTracker::clear()
{
	d->mKeys.clear();
	d->mModifiers = Qt::NoModifier;
}

bool KeyTracker::eventFilter(QObject *, QEvent *event)
{
	switch (event->type()) {
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			d->keyEvent(static_cast<QKeyEvent *>(event));
			break;
		default:
			break;
	}
	return false;
}

} // namespace PaintField
