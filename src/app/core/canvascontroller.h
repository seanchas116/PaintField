#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QWidget>
#include <QPointer>

#include "canvasview.h"
#include "actionmanager.h"

namespace PaintField
{

template <class T>
class ScopedQObjectPointer
{
public:
	
	ScopedQObjectPointer() {}
	ScopedQObjectPointer(T *p) : _p(p) {}
	
	~ScopedQObjectPointer()
	{
		if (_p)
			reinterpret_cast<QObject *>(_p.data())->deleteLater();
	}
	
	T *data() const { return _p; }
	bool isNull() const { return data(); }
	void reset(T *p) { _p = p; }
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
	
	QPointer<T> _p;
};

class WorkspaceController;

class CanvasController : public QObject
{
	Q_OBJECT
public:
	
	/**
	 * Constructs a canvas controller.
	 * @param document
	 * @param parent
	 */
	CanvasController(Document *document, WorkspaceController *parent = 0);
	
	/**
	 * Shows a dialog, creates a new file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromNew(WorkspaceController *parent = 0);
	
	/**
	 * Shows a dialog, opens a file and creates a controller from it.
	 * @param parent The new controller's parent
	 * @return The created controller
	 */
	static CanvasController *fromOpen(WorkspaceController *parent = 0);
	
	/**
	 * @return The workspace controller which have the canvas controller
	 */
	WorkspaceController *workspaceController() { return reinterpret_cast<WorkspaceController *>(parent()); }
	
	/**
	 * @return The canvas view
	 */
	CanvasView *view() { return _view.data(); }
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document() { return _view->document(); }
	
	/**
	 * @return The actions which belongs to the controller.
	 */
	QList<QAction *> actions() { return _actions; }
	
	ActionManager *actionManager() { return _actionManager; }
	
signals:
	
public slots:
	
	bool saveAsCanvas();
	bool saveCanvas();
	bool closeCanvas();
	
private:
	
	ScopedQObjectPointer<CanvasView> _view;
	ActionManager *_actionManager;
	QList<QAction *> _actions;
};

}

#endif // CANVASCONTROLLER_H
