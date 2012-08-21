#ifndef FSGUIMAIN_H
#define FSGUIMAIN_H

#include <QObject>
#include "fsactionmanager.h"
#include "fscanvasview.h"

class QMenuBar;
class FSPanel;

class FSGuiMain : public QObject
{
	Q_OBJECT
public:
	explicit FSGuiMain(QObject *parent = 0);
	
	void addPanel(QWidget *panel);
	QList<FSPanel *> panels();
	
	void addCanvasView(FSCanvasView *view);
	FSCanvasView *currentView() { return _currentView; }
	FSDocumentModel *currentDocument() { return _currentView ? _currentView->documentModel() : 0; }
	bool isViewSelected() const { return _currentView; }
	
	FSActionManager *actionManager() { return _actionManager; }
	QAction *action(const QString &name) { return _actionManager->action(name); }
	
	static FSGuiMain *instance() { return _instance; }
	
signals:
	
	void currentViewChanged(FSCanvasView *view);
	void currentDocumentChanged(FSDocumentModel *document);
	
public slots:
	
	void setCurrentView(FSCanvasView *view);
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
	
	void viewActivated();
	void viewClosed();
	
private:
	
	QList<FSPanel *> _panels;
	
	QList<FSCanvasView *> _views;
	FSCanvasView *_currentView;
	
	QMenuBar *_mainMenu;
	FSActionManager *_actionManager;
	
	static FSGuiMain *_instance;
};

inline FSGuiMain *fsGuiMain() { return FSGuiMain::instance(); }
inline FSActionManager *fsActionManager() { return FSGuiMain::instance()->actionManager(); }

#endif // FSGUIMAIN_H
