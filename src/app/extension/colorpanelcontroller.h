#ifndef COLORPANELCONTROLLER_H
#define COLORPANELCONTROLLER_H



namespace PaintField
{

class ColorPanelController : public PanelController
{
	Q_OBJECT
public:
	explicit ColorPanelController(WorkspaceController *controller, QObject *parent = 0);
	
	QWidget *createView(QWidget *parent);
	
signals:
	
public slots:
	
private:
	
	WorkspaceController *_workspace;
};

}

#endif // COLORPANELCONTROLLER_H
