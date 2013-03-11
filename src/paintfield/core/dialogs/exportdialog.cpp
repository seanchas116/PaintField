#include <QPushButton>

#include "../widgets/widgetgroup.h"

#include "exportdialog.h"
#include "ui_exportdialog.h"

namespace PaintField
{

ExportDialog::ExportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PaintField_ExportDialog),
	_qualityGroup(new WidgetGroup(this))
{
	setWindowTitle(tr("Export"));
	
	ui->setupUi(this);
	
	_qualityGroup->addWidget(ui->qualityLabel);
	_qualityGroup->addWidget(ui->horizontalSlider);
	_qualityGroup->addWidget(ui->spinBox);
	_qualityGroup->addWidget(ui->percentLabel);
	
	connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onQualityValueChanged(int)));
	
	ui->spinBox->setValue(90);
	
	_itemsToFormats[new QListWidgetItem(tr("PNG"), ui->listWidget)] = "png";
	_itemsToFormats[new QListWidgetItem(tr("JPEG"), ui->listWidget)] = "jpg";
	_itemsToFormats[new QListWidgetItem(tr("Windows Bitmap"), ui->listWidget)] = "bmp";
	
	connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onFormatItemChanged(QListWidgetItem*)));
	ui->listWidget->setCurrentItem(0);
}

ExportDialog::~ExportDialog()
{
	delete ui;
}

QString ExportDialog::currentText() const
{
	auto item = ui->listWidget->currentItem();
	return item ? item->text() : QString();
}

void ExportDialog::onFormatItemChanged(QListWidgetItem *item)
{
	_format = _itemsToFormats[item];
	_qualityGroup->setVisible(_format == "jpg");
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(item);
}

void ExportDialog::onQualityValueChanged(int quality)
{
	_quality = quality;
}

}

