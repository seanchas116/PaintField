#include "jpegexportform.h"
#include "ui_jpegexportform.h"

namespace PaintField {

JpegExportForm::JpegExportForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::JpegExportForm)
{
	ui->setupUi(this);
}

JpegExportForm::~JpegExportForm()
{
	delete ui;
}

int JpegExportForm::quality() const
{
	return ui->spinBox->value();
}

} // namespace PaintField
