#ifndef FSBRUSHSETTINGWIDGET_H
#define FSBRUSHSETTINGWIDGET_H

#include "brushsetting.h"

namespace PaintField
{

class BrushSettingPanel : public QWidget
{
	Q_OBJECT
	
public:
	explicit BrushSettingPanel(QWidget *parent = 0);
	
	QSize sizeHint() const;
	
private slots:
	
	void onDiameterValueChanged(double value);
	
private:
	
	BrushSetting *_setting;
};

}

#endif // FSBRUSHSETTINGWIDGET_H
