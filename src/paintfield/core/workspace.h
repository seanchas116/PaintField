#pragma once

#include <QObject>
#include <QPointer>

#include "extension.h"
#include "global.h"

namespace PaintField
{

class Canvas;
class WorkspaceView;
class WorkspaceExtension;
class ToolManager;
class PaletteManager;
class CanvasSplitAreaController;
class Document;
class ColorButtonGroup;

class Workspace : public QObject
{
	Q_OBJECT
public:
	explicit Workspace(QObject *parent = 0);
	~Workspace();
	
	ToolManager *toolManager();
	PaletteManager *paletteManager();
	ColorButtonGroup *colorButtonGroup();
	
	void setView(WorkspaceView *view);
	WorkspaceView *view();
	
	void addExtensions(const WorkspaceExtensionList &extensions);
	WorkspaceExtensionList extensions();
	
	void addActions(const QActionList &actions);
	QActionList actions();
	
	void addNullCanvasExtensions(const CanvasExtensionList &extensions);
	CanvasExtensionList nullCanvasExtensions();
	
	void addNullCanvasActions(const QActionList &actions);
	QActionList nullCanvasActions();
	
	void addAndShowCanvas(Canvas *canvas);
	void addAndShowDocument(Document *document);
	
	/**
	 * Adds a canvas.
	 * @param canvas
	 */
	void addCanvas(Canvas *canvas);
	
	QList<Canvas *> canvases();
	
	Canvas *currentCanvas();
	
	QActionList currentCanvasActions();
	CanvasExtensionList currentCanvasModules();
	
signals:
	
	void currentCanvasDocumentPropertyChanged();
	void canvasDocumentPropertyChanged(Canvas *canvas);
	
	void currentCanvasChanged(Canvas *canvas);
	
	void canvasShowRequested(Canvas *canvas);
	
	void canvasAboutToBeRemoved(Canvas *canvas);
	void focused();
	
	void shouldBeDeleted(Workspace *workspace);
	
	void splitVerticallyRequested();
	void splitHorizontallyRequested();
	void closeCurrentSplitRequested();
	
public slots:
	
	void setFocus();
	
	/**
	 * Sets the current canvas.
	 * @param canvas
	 */
	void setCurrentCanvas(Canvas *canvas);
	
	/**
	 * Creates a new document and adds a canvas of it.
	 */
	void newCanvas();
	
	void newCanvasFromImageFile();
	
	void newCanvasFromClipboard();
	
	/**
	 * Opens a document and adds a canvas of it.
	 */
	void openCanvas();
	
	void openCanvasFromFilepath(const QString &filepath);
	
	/**
	 * Try to close all canvases.
	 * @return If succeeded
	 */
	bool tryClose();
	
private slots:
	
	void deleteCanvas(Canvas *canvas);
	void onCanvasDocumentPropertyChanged();
	
private:
	
	struct Data;
	Data *d;
};

}

