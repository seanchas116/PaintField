#ifndef FSGUIMAIN_H
#define FSGUIMAIN_H

#include <QObject>
#include "fsactionmanager.h"
#include "fscanvas.h"
#include "fscore.h"

class QMenuBar;
class FSPanel;
class FSPanelWidget;

class FSGuiMain : public QObject
{
	Q_OBJECT
public:
	explicit FSGuiMain(QObject *parent = 0);
	
	void addPanel(FSPanelWidget *panel);
	QList<FSPanel *> panels();
	
	void addCanvas(FSCanvas *canvas);
	FSCanvas *currentCanvas() { return _currentCanvas; }
	FSDocumentModel *currentDocument() { return _currentCanvas ? _currentCanvas->document() : 0; }
	bool isViewSelected() const { return _currentCanvas; }
	
	FSActionManager *actionManager() { return _actionManager; }
	QAction *action(const QString &name) { return _actionManager->action(name); }
	
	static FSGuiMain *instance() { return _instance; }
	
signals:
	
	void currentCanvasChanged(FSCanvas *canvas);
	void currentDocumentChanged(FSDocumentModel *document);
	
public slots:
	
	void setCurrentCanvas(FSCanvas *canvas);
	bool quit();
	
private slots:
	
	void newFile();
	void openFile();
	void saveFile();
	void saveAsFile();
	void closeFile();
	void exportFile();
	
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
	
private slots:
	
	void onCanvasActivated();
	void onCanvasClosed();
	
private:
	
	QList<FSPanel *> _panels;
	
	QList<FSCanvas *> _canvases;
	FSCanvas *_currentCanvas;
	
	QMenuBar *_mainMenu;
	FSActionManager *_actionManager;
	
	static FSGuiMain *_instance;
};

inline FSGuiMain *fsGuiMain() { return FSGuiMain::instance(); }
inline FSActionManager *fsActionManager() { return FSGuiMain::instance()->actionManager(); }

#endif // FSGUIMAIN_H
