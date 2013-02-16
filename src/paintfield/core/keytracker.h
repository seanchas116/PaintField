#ifndef PAINTFIELD_KEYTRACKER_H
#define PAINTFIELD_KEYTRACKER_H

#include <QList>

class QKeySequence;

namespace PaintField {

class KeyTracker
{
public:
	
	KeyTracker() {}
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	QList<int> pressedKeys() const { return _keys; }
	
	bool match(const QKeySequence &sequence) const;
	
private:
	
	QList<int> _keys;
};

} // namespace PaintField

#endif // PAINTFIELD_KEYTRACKER_H
