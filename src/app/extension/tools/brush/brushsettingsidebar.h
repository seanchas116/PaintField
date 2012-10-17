#ifndef FSBRUSHSETTINGWIDGET_H
#define FSBRUSHSETTINGWIDGET_H

#include "brushsetting.h"
#include <QWidget>

namespace PaintField
{

class BrushSettingPanel : public QWidget
{
	Q_OBJECT
	
public:
	explicit BrushSettingPanel(QWidget *parent = 0);
	
private slots:
	
	void onDiameterValueChanged(double value);
	
private:
	
	BrushSetting *_setting;
};

}

#endif // FSBRUSHSETTINGWIDGET_H
