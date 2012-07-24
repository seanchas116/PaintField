#ifndef FSMAINPANEL_H
#define FSMAINPANEL_H

#include <QWidget>

class QMenu;
class FSCanvasView;

class FSMainPanel : public QWidget
{
	Q_OBJECT
public:
	
	explicit FSMainPanel(QWidget *parent = 0);
	
	void addCanvasView(FSCanvasView *view);
	
	static FSMainPanel *instance() { return _instance; }
	
signals:
	
public slots:
	
protected:
	
	void closeEvent(QCloseEvent *);
	
private slots:
	
private:
	
	static FSMainPanel *_instance;
	
	QList<FSCanvasView *> _views;
	FSCanvasView *_currentView;
	QMenu *_menu;
};

inline FSMainPanel *fsMainPanel() { return FSMainPanel::instance(); }

#endif // FSMAINPANEL_H
