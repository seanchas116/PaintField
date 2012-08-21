#include "fsbrushsettingwidget.h"
#include "ui_fsbrushsettingwidget.h"

FSBrushSettingWidget::FSBrushSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FSBrushSettingWidget),
    _setting(0)
{
	ui->setupUi(this);
}

FSBrushSettingWidget::~FSBrushSettingWidget()
{
	delete ui;
}

void FSBrushSettingWidget::on_spinBox_valueChanged(int arg1)
{
	if (_setting)
	{
		_setting->diameter = arg1;
	}
}
