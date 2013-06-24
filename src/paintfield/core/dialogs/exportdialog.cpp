#include <QPushButton>
#include <QHBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>

#include "../widgets/widgetgroup.h"

#include "exportdialog.h"

namespace PaintField
{

JpegForm::JpegForm(QWidget *parent) :
    QWidget(parent)
{
	auto layout = new QFormLayout();
	
	{
		auto hlayout = new QHBoxLayout();
		
		auto spin = new QSpinBox();
		spin->setRange(0, 100);
		auto slider = new QSlider(Qt::Horizontal);
		slider->setRange(0, 100);
		
		connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
		connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
		connect(spin, SIGNAL(valueChanged(int)), this, SLOT(onQualityChanged(int)));
		
		spin->setValue(_quality);
		
		hlayout->addWidget(slider);
		hlayout->addWidget(spin);
		hlayout->addWidget(new QLabel("%"));
		
		layout->addRow(tr("Quality"), hlayout);
	}
	
	setLayout(layout);
}

PngForm::PngForm(QWidget *parent) :
    QWidget(parent)
{
	auto layout = new QFormLayout();
	
	{
		auto check = new QCheckBox(tr("Alpha Channel"));
		connect(check, SIGNAL(toggled(bool)), this, SLOT(onAlphaChanged(bool)));
		check->setChecked(_alphaEnabled);
		layout->addRow(check);
	}
	
	setLayout(layout);
}

ExportDialog::ExportDialog(QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle(tr("Export"));
	
	auto mainHLayout = new QHBoxLayout();
	
	{
		_listWidget = new QListWidget();
		_listWidget->setMaximumWidth(150);
		
		_formatsForItems[new QListWidgetItem(tr("PNG"), _listWidget)] = "png";
		_formatsForItems[new QListWidgetItem(tr("JPEG"), _listWidget)] = "jpg";
		_formatsForItems[new QListWidgetItem(tr("Windows Bitmap"), _listWidget)] = "bmp";
		
		connect(_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onFormatItemChanged(QListWidgetItem*)));
		
		mainHLayout->addWidget(_listWidget, 0);
	}
	
	{
		auto vlayout = new QVBoxLayout();
		
		_jpegForm = new JpegForm();
		vlayout->addWidget(_jpegForm);
		
		_pngForm = new PngForm();
		vlayout->addWidget(_pngForm);
		
		vlayout->addStretch(1);
		
		{
			_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
			connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
			connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
			vlayout->addWidget(_buttonBox);
		}
		
		mainHLayout->addLayout(vlayout, 1);
	}
	
	setLayout(mainHLayout);
	
	_listWidget->selectionModel()->select(_listWidget->model()->index(0,0), QItemSelectionModel::SelectCurrent);
	
	resize(500, 250);
}

QString ExportDialog::currentText() const
{
	auto item = _listWidget->currentItem();
	return item ? item->text() : QString();
}

void ExportDialog::onFormatItemChanged(QListWidgetItem *item)
{
	_format = _formatsForItems[item];
	
	_jpegForm->setVisible(false);
	_pngForm->setVisible(false);
	
	_jpegForm->setVisible(_format == "jpg");
	_pngForm->setVisible(_format == "png");
	
	_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(item);
}

}

