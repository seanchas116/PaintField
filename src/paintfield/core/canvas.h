#pragma once

#include <QObject>
#include <QPointer>

#include "document.h"
#include "extension.h"

class QItemSelectionModel;

namespace PaintField
{

class CanvasExtension;
class Workspace;

/**
 * The Canvas is a context of document editing, which is displayed in each tabs.
 */
class Canvas : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
	Q_PROPERTY(double rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
	Q_PROPERTY(QPoint translation READ translation WRITE setTranslation NOTIFY translationChanged)
	Q_PROPERTY(bool mirrored READ isMirrored WRITE setMirrored NOTIFY mirroredChanged)
	
public:
	
	/**
	 * Constructs a canvas.
	 * @param document
	 * @param parent
	 */
	Canvas(Document *document, Workspace *parent);
	
	Canvas(Canvas *other, Workspace *parent);
	
	~Canvas();
	
	/**
	 * @return The workspace controller which owns the canvas
	 */
	Workspace *workspace();
	
	/**
	 * @return The document the canvas handles
	 */
	Document *document();
	
	/**
	 * Adds actions that will belong to the canvas.
	 * The actions will be shown in the menubar.
	 * @param actions
	 */
	void addActions(const QActionList &actions);
	
	/**
	 * @return The actions which belongs to the canvas.
	 */
	QActionList actions();
	
	void addExtensions(const CanvasExtensionList &extensions);
	CanvasExtensionList extensions();
	
	/**
	 * This function must be used only by CanvasView.
	 * @param view
	 */
	void setView(CanvasView *view);
	
	CanvasView *view();
	
	virtual void onSetCurrent();
	
	/**
	 * @return The viewport scale (1 = actual)
	 */
	double scale() const;
	
	/**
	 * @return The viewport rotation (degrees)
	 */
	double rotation() const;
	
	/**
	 * @return The viewport translation (pixels)
	 */
	QPoint translation() const;
	
	bool isMirrored() const;
	bool isRetinaMode() const;
	
	void memorizeNavigation();
	void restoreNavigation();
	
	/**
	 * @return The current tool
	 */
	Tool *tool();
	
signals:
	
	void documentPropertyChanged();
	
	void shouldBeDeleted(Canvas *canvas);
	
	void scaleChanged(double scale);
	void rotationChanged(double rotation);
	void translationChanged(const QPoint &translation);
	void mirroredChanged(bool mirrored);
	void retinaModeChanged(bool mode);
	
	void toolChanged(Tool *tool);
	
public slots:
	
	/**
	 * Shows a warning dialog and emits shouldBeClosed() if not cancelled.
	 * @return false if cancelled
	 */
	bool closeCanvas();
	
	/**
	 * Creates and adds a new canvas that shares the document with this canvas.
	 */
	void newCanvasIntoDocument();
	
	void setScale(double scale);
	void setRotation(double rotation);
	void setTranslation(const QPoint &translation);
	void setTranslation(int x, int y) { setTranslation(QPoint(x, y)); }
	void setTranslationX(int x) { setTranslation(x, translation().y()); }
	void setTranslationY(int y) { setTranslation(translation().x(), y); }
	
	void setMirrored(bool mirrored);
	void setRetinaMode(bool mode);
	
private slots:
	
	void onToolChanged(const QString &name);
	
private:
	
	void commonInit();
	
	struct Data;
	Data *d;
};

}

