#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QWidget>
#include <QPointer>

#include "canvas.h"

namespace PaintField
{

template <class T>
class ScopedGuardedPointer
{
public:
	
	ScopedGuardedPointer() {}
	ScopedGuardedPointer(T *p) : _p(p) {}
	
	~ScopedGuardedPointer()
	{
		if (_p) _p->deleleLater();
	}
	
	T *data() const { return _p; }
	bool isNull() const { return data(); }
	void reset(T *p) { _p = p; }
	
	
private:
	
	QPointer<T> _p;
};

class WorkspaceController;

class CanvasController : public QObject
{
	Q_OBJECT
public:
	CanvasController(Document *document, WorkspaceController *parent = 0);
	~CanvasController();
	
	static CanvasController *fromNew(WorkspaceController *parent = 0);
	static CanvasController *fromOpen(WorkspaceController *parent = 0);
	
	WorkspaceController *workspaceController() { return reinterpret_cast<WorkspaceController *>(parent()); }
	
	Canvas *canvas() { return _canvas; }
	Document *document() { return _canvas->document(); }
	QList<QWidget *> panels() { return _panels; }
	QList<QAction *> actions() { return _actions; }
	
	void addAction(QAction *action)
	{
		_canvas->addAction(action);
		_actions << action;
	}
	void addAction(QAction *action, const QString &id)
	{
		action->setObjectName(id);
		addAction(action);
	}
	void addAction(QAction *action, const QString &id, const QObject *receiver, const char *triggerSlot)
	{
		connect(action, SIGNAL(triggered()), receiver, triggerSlot);
		addAction(action, id);
	}
	
	void addPanel(QWidget *panel) { _panels << panel; }
	void addPanel(QWidget *panel, const QString &id)
	{
		panel->setObjectName(id);
		addPanel(panel);
	}
	
signals:
	
public slots:
	
	bool saveAsCanvas();
	bool saveCanvas();
	bool closeCanvas();
	
private:
	
	QPointer<Canvas> _canvas;
	QList<QWidget *> _panels;
	QList<QAction *> _actions;
};

}

#endif // CANVASCONTROLLER_H
