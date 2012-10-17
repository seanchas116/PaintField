#ifndef PANELFACTORY_H
#define PANELFACTORY_H

#include <QObject>

class QWidget;

namespace PaintField
{

class WorkspaceController;
class CanvasController;

/**
 * The SidebarFactory creates a sidebar, which is docked in the side of workspaces.
 */
class SidebarFactory : public QObject
{
	Q_OBJECT
	
public:
	
	/**
	 * This enum describes when the sidebar should be created.
	 */
	enum CreationType
	{
		/**
		 * The sidebar should be created when a workspace added
		 */
		CreationTypeWorkspace,
		/**
		 * The sidebar should be created when the current canvas is changed
		 */
		CreationTypeCanvas
	};
	
	explicit SidebarFactory(QObject *parent = nullptr) : QObject(parent) {}
	
	/**
	 * Sets the text (window title) of the sidebar.
	 * @param text
	 */
	void setText(const QString &text) { _text = text; }
	QString text() const { return _text; }
	
	void setCreationType(CreationType type) { _type = type; }
	CreationType creationType() { return _type; }
	
	/**
	 * Creates a sidebar.
	 * This function is called when a new workspace is added, only if creationType() == CreationTypeWorkspace.
	 * The default inplementation returns 0 (creates nothing).
	 * @param workspace
	 * @param parent
	 * @return 
	 */
	virtual QWidget *createSidebar(WorkspaceController *workspace, QWidget *parent) { Q_UNUSED(workspace) Q_UNUSED(parent) return nullptr; }
	
	/**
	 * Creates a sidebar.
	 * This function is called when the current canvas is changed, only if creationType() == CreationTypeCanvas.
	 * The default inplementation returns 0 (creates nothing).
	 * @param canvas
	 * @param parent
	 * @return 
	 */
	virtual QWidget *createSidebarForCanvas(CanvasController *canvas, QWidget *parent) { Q_UNUSED(canvas) Q_UNUSED(parent) return nullptr; }
	
signals:
	
public slots:
	
private:
	
	QString _text;
	CreationType _type = CreationTypeWorkspace;
};

}

#endif // PANELFACTORY_H
