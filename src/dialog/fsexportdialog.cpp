#include "fswidgetgroup.h"

#include "fsexportdialog.h"
#include "ui_fsexportdialog.h"

FSExportDialog::FSExportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FSExportDialog),
	_group(new FSWidgetGroup(this))
{
	setWindowTitle(tr("Export"));
	
	ui->setupUi(this);
	
	_group->addWidget(ui->qualityLabel);
	_group->addWidget(ui->horizontalSlider);
	_group->addWidget(ui->spinBox);
	_group->addWidget(ui->percentLabel);
	
	connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onQualityValueChanged(int)));
	
	_hash.insert("png", tr("PNG (*.png)"));
	_hash.insert("jpg", tr("JPEG (*.jpg)"));
	_hash.insert("bmp", tr("Windows Bitmap (*.bmp)"));
	
	ui->comboBox->addItems(_hash.values());
	
	connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxActivated(int)));
	
	ui->comboBox->setCurrentIndex(0);
	onComboBoxActivated(0);
	ui->spinBox->setValue(90);
}

FSExportDialog::~FSExportDialog()
{
	delete ui;
}

QString FSExportDialog::currentText() const
{
	return ui->comboBox->currentText();
}

void FSExportDialog::onComboBoxActivated(int index)
{
	_format = _hash.keys().at(index);
	
	_group->setVisible(_format == "jpg");
}

void FSExportDialog::onQualityValueChanged(int quality)
{
	_quality = quality;
}

