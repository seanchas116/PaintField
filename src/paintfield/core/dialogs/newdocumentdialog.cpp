#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <cmath>
#include <QLabel>

#include "newdocumentdialog.h"

namespace PaintField
{

struct NewDocumentDialog::Data
{
	QSpinBox *widthSpin = nullptr;
	QSpinBox *heightSpin = nullptr;
	QCheckBox *keepRatioCheck = nullptr;
	double proportion = 1.0;
};

NewDocumentDialog::NewDocumentDialog(QWidget *parent) :
    QDialog(parent),
    d(new Data)
{
	resize(300, 160);
	setWindowTitle(tr("New Document"));
	
	auto mainLayout = new QVBoxLayout();
	
	{
		auto hlayout = new QHBoxLayout();
		
		auto layout = new QFormLayout();
		
		d->widthSpin = new QSpinBox();
		d->widthSpin->setRange(0, 99999);
		layout->addRow(tr("Width"), d->widthSpin);
		connect(d->widthSpin, SIGNAL(valueChanged(int)), this, SLOT(onWidthChanged(int)));
		
		d->heightSpin = new QSpinBox();
		d->heightSpin->setRange(0, 99999);
		layout->addRow(tr("Height"), d->heightSpin);
		connect(d->heightSpin, SIGNAL(valueChanged(int)), this, SLOT(onHeightChanged(int)));
		
		d->keepRatioCheck = new QCheckBox(tr("Keep Aspect Ratio"));
		layout->addRow(d->keepRatioCheck);
		
		d->widthSpin->setValue(1600);
		d->heightSpin->setValue(1200);
		d->keepRatioCheck->setChecked(false);
		
		setProportion();
		
		hlayout->addStretch(1);
		hlayout->addLayout(layout);
		hlayout->addStretch(1);
		
		mainLayout->addLayout(hlayout);
		//mainLayout->addLayout(layout);
	}
	
	{
		auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
		
		mainLayout->addWidget(buttonBox);
	}
	
	mainLayout->setAlignment(Qt::AlignHCenter);
	setLayout(mainLayout);
}

QSize NewDocumentDialog::documentSize() const
{
	return QSize(d->widthSpin->value(), d->heightSpin->value());
}

void NewDocumentDialog::onWidthChanged(int w)
{
	if (d->keepRatioCheck->isChecked())
	{
		d->heightSpin->setValue( std::round(w / d->proportion ) );
	}
	else
	{
		setProportion();
	}
}

void NewDocumentDialog::onHeightChanged(int h)
{
	if (d->keepRatioCheck->isChecked())
	{
		d->widthSpin->setValue( std::round(h * d->proportion ) );
	}
	else
	{
		setProportion();
	}
}

void NewDocumentDialog::setProportion()
{
	d->proportion = double(d->widthSpin->value()) / double(d->heightSpin->value());
}

NewDocumentDialog::~NewDocumentDialog()
{
	delete d;
}

}
