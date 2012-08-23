#ifndef FSTOOLSETTINGPANEL_H
#define FSTOOLSETTINGPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include "fspanel.h"

class FSToolFactory;

class FSToolSettingPanel : public FSPanelWidget
{
	Q_OBJECT
public:
	explicit FSToolSettingPanel(QWidget *parent = 0);
	
	QSize sizeHint() const;
	
signals:
	
public slots:
	
private slots:
	
	void onCurrentToolFactoryChanged(FSToolFactory *factory);
	
private:
	
	QVBoxLayout *_layout;
	QScopedPointer<QWidget> _widget;
};

#endif // FSTOOLSETTINGPANEL_H
