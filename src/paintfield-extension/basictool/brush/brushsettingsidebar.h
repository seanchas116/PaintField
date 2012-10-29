#ifndef FSBRUSHSETTINGWIDGET_H
#define FSBRUSHSETTINGWIDGET_H

#include "brushsetting.h"
#include <QWidget>

namespace PaintField
{

class BrushSettingSidebar : public QWidget
{
	Q_OBJECT
	
public:
	explicit BrushSettingSidebar(BrushSetting *setting, QWidget *parent = 0);
	
private slots:
	
	void onDiameterValueChanged(double value);
	
private:
	
	BrushSetting *_setting;
};

}

#endif // FSBRUSHSETTINGWIDGET_H
