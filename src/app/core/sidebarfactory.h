#ifndef PANELFACTORY_H
#define PANELFACTORY_H

#include <QObject>

class QWidget;

namespace PaintField
{

class WorkspaceController;
class CanvasController;

class SidebarFactory : public QObject
{
	Q_OBJECT
	
public:
	explicit SidebarFactory(QObject *parent = nullptr) : QObject(parent) {}
	
	void setText(const QString &text) { _text = text; }
	QString text() const { return _text; }
	
	virtual QWidget *createSidebar(WorkspaceController *workspace, QWidget *parent) { Q_UNUSED(workspace) Q_UNUSED(parent) return nullptr; }
	virtual QWidget *createSidebarForCanvas(CanvasController *canvas, QWidget *parent) { Q_UNUSED(canvas) Q_UNUSED(parent) return nullptr; }
	
signals:
	
public slots:
	
private:
	
	QString _text;
};

}

#endif // PANELFACTORY_H
