#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include "../formatsupport.h"

#include "formatexportdialog.h"

namespace PaintField {

struct FormatExportDialog::Data
{
	QVBoxLayout *layout = nullptr;
	QComboBox *comboBox = nullptr;
	
	QHash<int, FormatSupport *> formatSupports;
	FormatSupport *currentFormatSupport = nullptr;
	
	QHash<FormatSupport *, QWidget *> optionWidget;
	QWidget *currentSettingsWidget = nullptr;
};

FormatExportDialog::FormatExportDialog(const QList<FormatSupport *> &formatSupports, QWidget *parent) :
	QDialog(parent),
	d(new Data)
{
	{
		auto layout = new QVBoxLayout();
		
		{
			auto cbox = new QComboBox();
			layout->addWidget(cbox);
			d->comboBox = cbox;
		}
		
		{
			auto bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
			connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
			connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
			layout->addWidget(bbox);
		}
		
		setLayout(layout);
		d->layout = layout;
	}
	
	for (auto format : formatSupports)
	{
		int index = d->comboBox->count();
		d->formatSupports[index] = format;
		d->comboBox->addItem(format->shortDescription());
		
		auto widget = format->createExportOptionWidget();
		
		if (widget)
		{
			if (d->optionWidget.contains(format))
				d->optionWidget[format]->deleteLater();
			
			d->optionWidget[format] = widget;
		}
	}
	
	d->comboBox->setCurrentIndex(0);
	onCurrentIndexChanged(0);
	
	connect(d->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

FormatExportDialog::~FormatExportDialog()
{
	delete d;
}

FormatSupport *FormatExportDialog::currentFormatSupport()
{
	return d->currentFormatSupport;
}

QWidget *FormatExportDialog::currentOptionWidget()
{
	return d->currentSettingsWidget;
}

void FormatExportDialog::onCurrentIndexChanged(int index)
{
	auto format = d->formatSupports.value(index, nullptr);
	Q_ASSERT(format);
	d->currentFormatSupport = format;
	
	auto settingsWidget = d->optionWidget.value(format, nullptr);
	
	// remove old widget
	if (d->currentSettingsWidget)
	{
		d->layout->removeWidget(d->currentSettingsWidget);
		d->currentSettingsWidget->setVisible(false);
	}
	
	d->currentSettingsWidget = settingsWidget;
	
	if (settingsWidget)
	{
		d->layout->insertWidget(1, settingsWidget);
		settingsWidget->setVisible(true);
	}
	
	// set height to minimum
	if (this->isVisible())
	{
		auto geom = this->geometry();
		geom.setHeight(1);
		this->setGeometry(geom);
	}
}



} // namespace PaintField
