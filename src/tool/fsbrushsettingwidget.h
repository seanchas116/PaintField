#ifndef FSBRUSHSETTINGWIDGET_H
#define FSBRUSHSETTINGWIDGET_H

#include "fspanel.h"

#include "fsbrushsetting.h"

class FSBrushSettingWidget : public FSPanelWidget
{
	Q_OBJECT
	
public:
	explicit FSBrushSettingWidget(QWidget *parent = 0);
	
	QSize sizeHint() const;
	
private slots:
	
	void onDiameterValueChanged(double value);
	
private:
	
	FSBrushSetting *_setting;
};

#endif // FSBRUSHSETTINGWIDGET_H
