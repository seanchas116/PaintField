#pragma once

#include <QObject>
#include <QSet>

class QKeySequence;

namespace PaintField {

class KeyTracker : public QObject
{
	Q_OBJECT
	
public:
	
	explicit KeyTracker(QObject *parent);
	~KeyTracker();
	
	void pressKey(int key);
	void releaseKey(int key);
	
	QSet<int> pressedKeys() const;
	QSet<int> pressedKeysWithModifiers() const;
	Qt::KeyboardModifiers modifiers() const;
	
	bool match(const QKeySequence &sequence) const;
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

