#pragma once

#include <QObject>
#include <QPointer>
#include <Malachite/Affine2D>

#include "document.h"
#include "extension.h"

class QItemSelectionModel;

namespace PaintField
{

class CanvasViewController;
class CanvasExtension;
class Workspace;

struct CanvasTransforms
{
	double scale = 1.0;
	double rotation = 0.0;
	QPoint translation;
	bool mirrored = false;
	bool retinaMode = false;
	QSize sceneSize;
	QSize viewSize;
	
	QTransform viewToScene, sceneToView, windowToScene, sceneToWindow, viewToMipmap, mipmapToView;
	
	double mipmapScale = 0;
	int mipmapLevel = 0;
	QSize mipmapSceneSize;
};

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
	
	// actions and extensions
	
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
	
	// view & controller
	
	/**
	 * This function must be used only by CanvasView.
	 * @param controller
	 */
	void setViewController(CanvasViewController *controller);
	
	CanvasViewController *viewController();
	
	CanvasView *view();
	
	virtual void onSetCurrent();
	
	// transform
	
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
	
	/**
	 * @return Whether the viewport is mirrored
	 */
	bool isMirrored() const;
	
	/**
	 * @return Whether the viewport is Retina mode
	 */
	bool isRetinaMode() const;
	
	std::shared_ptr<const CanvasTransforms> transforms() const;
	
	QPoint maxAbsoluteTranslation() const;
	
	/**
	 * Memorizes the current transform.
	 * Memorization can be performed only once.
	 * If you try to perform memorization more than once, the memorization will be overwritten.
	 */
	void memorizeNavigation();
	
	/**
	 * Restores the transform from the memorization.
	 */
	void restoreNavigation();
	
	QSize viewSize() const;
	
	/**
	 * @return The current tool
	 */
	Tool *tool();
	
signals:
	
	void documentPropertyChanged();
	
	/**
	 * Emitted when close of the canvas is accepted.
	 * Usually, when this signal is emitted, the workspace will delete the canvas.
	 * @param canvas
	 */
	void shouldBeDeleted(Canvas *canvas);
	
	void scaleChanged(double scale);
	void rotationChanged(double rotation);
	void translationChanged(const QPoint &translation);
	void mirroredChanged(bool mirrored);
	void retinaModeChanged(bool mode);
	
	void transformsChanged(const std::shared_ptr<const CanvasTransforms> &transforms);
	
	/**
	 * Emitted when the transform is changed, before scale, rotation ... changed signals are emitted.
	 * @param transformToScene
	 * @param transformToView
	 */
	//void transformChanged(const Malachite::Affine2D &transformToScene, const Malachite::Affine2D &transformToView);
	
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
	
	void setViewSize(const QSize &size);
	
private slots:
	
	void onToolChanged(const QString &name);
	
private:
	
	void updateTransforms();
	void commonInit();
	
	struct Data;
	Data *d;
};

}

