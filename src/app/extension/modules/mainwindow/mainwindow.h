#ifndef FSMAINWINDOW_H
#define FSMAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QVBoxLayout;

namespace PaintField {

class PanelFrame : public QWidget
{
	Q_OBJECT
public:
	PanelFrame(QWidget *parent = 0);
	
	void setPanel(QWidget *panel);
	QWidget *panel() { return _panel; }
	
signals:
	
public slots:
	
protected:
	
	void closeEvent(QCloseEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
private:
	
	QWidget *_panel;
	QLabel *_label;
	QVBoxLayout *_layout;
	
	QPoint _origPanelPos, _origMousePos;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	
	void addPanel(PanelFrame *frame) { _panelFrames << frame; }
	void addPanel(QWidget *panel);
	
	void arrangeMenus();
	void arrangePanels();
	
signals:
	
public slots:
	
private:
	
	void arrangePanelsInArea(Qt::DockWidgetArea area, const QVariantList &list);
	PanelFrame *findPanelFrame(const QString &id);
	
	QList<PanelFrame *> _panelFrames;
};

}

#endif // FSMAINWINDOW_H
