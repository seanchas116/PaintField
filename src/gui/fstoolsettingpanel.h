#ifndef FSTOOLSETTINGPANEL_H
#define FSTOOLSETTINGPANEL_H

#include <QWidget>
#include <QVBoxLayout>

class FSToolFactory;

class FSToolSettingPanel : public QWidget
{
	Q_OBJECT
public:
	explicit FSToolSettingPanel(QWidget *parent = 0);
	
signals:
	
public slots:
	
private slots:
	
	void onCurrentToolFactoryChanged(FSToolFactory *factory);
	
private:
	
	QVBoxLayout *_layout;
	QScopedPointer<QWidget> _widget;
};

#endif // FSTOOLSETTINGPANEL_H
