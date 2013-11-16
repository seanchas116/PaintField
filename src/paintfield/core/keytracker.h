#pragma once

#include <QObject>
#include <QSet>
#include "global.h"

class QKeySequence;
class QKeyEvent;

namespace PaintField {

class KeyTracker : public QObject
{
	Q_OBJECT
	
public:
	
	explicit KeyTracker(QObject *parent);
	~KeyTracker();
	
	QSet<int> unmodifiedPressedKeys() const;
	QSet<int> pressedKeys() const;
	Qt::KeyboardModifiers modifiers() const;
	
	bool matches(const QKeySequence &sequence) const;

	void clear();

signals:
	
	void pressedKeysChanged(const QSet<int> &pressedKeys);

protected:

	bool eventFilter(QObject *, QEvent *event) override;
	
private:
	
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

