#include "pngexportform.h"
#include "ui_pngexportform.h"

namespace PaintField {

PngExportForm::PngExportForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PngExportForm)
{
	ui->setupUi(this);
}

PngExportForm::~PngExportForm()
{
	delete ui;
}

bool PngExportForm::isAlphaEnabled() const
{
	return ui->alphaCheckBox->isChecked();
}

} // namespace PaintField
