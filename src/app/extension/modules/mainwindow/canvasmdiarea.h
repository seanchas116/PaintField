#ifndef FSCANVASMDIAREA_H
#define FSCANVASMDIAREA_H

#include <QMdiArea>
#include <QMdiSubWindow>
#include "../documentUI/canvas.h"

namespace PaintField {

class CanvasMdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	CanvasMdiSubWindow(Document *document, QWidget *parent);
	
	Canvas *canvas() { return _canvas; }
	Document *document() { return _canvas->document(); }
	
signals:
	
	void windowHidden(CanvasMdiSubWindow *swindow);
	
protected:
	
	void closeEvent(QCloseEvent *closeEvent);
	void changeEvent(QEvent *changeEvent);
	
private:
	
	Canvas *_canvas;
};

class CanvasMdiArea : public QMdiArea
{
	Q_OBJECT
public:
	explicit CanvasMdiArea(QWidget *parent = 0);
	
signals:
	
	void currentDocumentChanged(Document *document);
	void documentVisibleChanged(Document *document, bool visible);
	
public slots:
	
	void addDocument(Document *document);
	void removeDocument(Document *document);
	void setDocumentVisible(Document *document, bool visible);
	void setCurrentDocument(Document *document);
	
private slots:
	
	void onSubWindowActivated(QMdiSubWindow *swindow);
	void onSubWindowHidden(CanvasMdiSubWindow *swindow);
	
private:
	
	CanvasMdiSubWindow *subWindowForDocument(Document *document);
	
	QList<CanvasMdiSubWindow *> _subWindows;
};

}

#endif // FSCANVASMDIAREA_H
