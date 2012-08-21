#ifndef FSBRUSHSETTINGWIDGET_H
#define FSBRUSHSETTINGWIDGET_H

#include <QWidget>

#include "fsbrushsetting.h"

namespace Ui {
class FSBrushSettingWidget;
}

class FSBrushSettingWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit FSBrushSettingWidget(QWidget *parent = 0);
	~FSBrushSettingWidget();
	
	void setBrushSetting(FSBrushSetting *setting) { _setting = setting; }
	
private slots:
	void on_spinBox_valueChanged(int arg1);
	
private:
	Ui::FSBrushSettingWidget *ui;
	
	FSBrushSetting *_setting;
};

#endif // FSBRUSHSETTINGWIDGET_H
