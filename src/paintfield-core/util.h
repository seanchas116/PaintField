#ifndef UTIL_H
#define UTIL_H

#include <QPointer>

#define PAINTFIELD_WARNING qWarning() << Q_FUNC_INFO << ":"
#define PAINTFIELD_DEBUG qDebug() << Q_FUNC_INFO << ":"


class QAction;
typedef QList<QAction *> QActionList;

namespace PaintField
{

/**
 * Creates an action.
 * @param id The object name
 * @param parent
 * @return 
 */
QAction *createAction(const QString &id, QObject *parent);

/**
 * Creates an action and connect the signal "triggered()".
 * @param id The object name
 * @param receiver The object which receives the slot
 * @param onTriggeredSlot The slot which receives the slot
 * @param parent
 * @return 
 */
QAction *createAction(const QString &id, QObject *receiver, const char *onTriggeredSlot, QObject *parent);

/**
 * Creates an action and connect the signal "triggered()".
 * The parent is set to receiver.
 * @param id
 * @param receiver
 * @param onTriggeredSlot
 * @return 
 */
inline QAction *createAction(const QString &id, QObject *receiver, const char *onTriggeredSlot) { return createAction(id, receiver, onTriggeredSlot, receiver); }

/**
 * Finds an QObject with objectName == id from the list.
 * @param list
 * @param id
 * @return found object or 0 if not found
 */
template <class T>
T *findQObject(const QList<T *> &list, const QString &id)
{
	QListIterator<T *> iter(list);
	
	while (iter.hasNext())
	{
		T *p = iter.next();
		
		if (p->objectName() == id)
			return p;
	}
	return 0;
}

template <class T>
T *findQObjectReverse(const QList<T *> &list, const QString &id)
{
	QListIterator<T *> iter(list);
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		T *p = iter.previous();
		
		if (p->objectName() == id)
			return p;
	}
	return 0;
}


template <class T>
class ScopedQObjectPointer
{
public:
	
	ScopedQObjectPointer() {}
	ScopedQObjectPointer(T *p) : _p(p) {}
	
	~ScopedQObjectPointer()
	{
		destroy();
	}
	
	T *data() const { return _p; }
	
	bool isNull() const { return data(); }
	
	void reset(T *p = 0)
	{
		destroy();
		_p = p;
	}
	
	void swap(ScopedQObjectPointer<T> &other)
	{
		T *p = _p;
		_p = other._p;
		other._p = p;
	}
	
	T *take()
	{
		T *p = _p;
		_p = 0;
		return p;
	}
	
	operator bool() const { return _p; }
	bool operator!() const { return !_p; }
	T &operator*() const { return *_p; }
	T *operator->() const { return _p; }
	
private:
	
	void destroy()
	{
		if (_p) _p->deleteLater();
		_p = 0;
	}
	
	QPointer<T> _p;
};

class GuardedQObjectList : public QObject
{
	Q_OBJECT
	
public:
	
	GuardedQObjectList(QObject *parent = 0) : QObject(parent) {}
	
	void append(QObject *object)
	{
		connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(onObjectDestroyed(QObject*)));
		_list << object;
	}
	
	QObjectList list() { return _list; }
	
private slots:
	
	void onObjectDestroyed(QObject *obj)
	{
		_list.removeAll(obj);
	}
	
private:
	
	QObjectList _list;
};

/**
 * Load a JSON from a file.
 * @param path
 * @return Parsed data (tree of QVariantList and QVariantMap)
 */
QVariant loadJsonFromFile(const QString &path);

void applyMacSmallSize(QWidget *widget);

QString unduplicatedName(const QStringList &existingNames, const QString &newName);

class ReproductiveInterface
{
public:
	virtual ~ReproductiveInterface() {}
	
	virtual QObject *createNew() = 0;
	
	template <class T>
	T *createNewAs()
	{
		auto obj = createNew();
		auto as = qobject_cast<T *>(obj);
		Q_ASSERT(as);
		return as;
	}
	
};

}

Q_DECLARE_INTERFACE(PaintField::ReproductiveInterface, "PaintField.ReproductiveInterface")


#endif // UTIL_H
