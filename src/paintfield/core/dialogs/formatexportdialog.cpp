#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFormLayout>
#include "../formatsupport.h"
#include "../widgets/simplebutton.h"

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
	
	int optionWidgetIndex = 0;
	
	QHash<FormatSupport::Capability, QLabel *> capabilityLabels;
	QHash<FormatSupport::Capability, SimpleButton *> capabilityButtons;
};

FormatExportDialog::FormatExportDialog(const QList<FormatSupport *> &formatSupports, QWidget *parent) :
	QDialog(parent),
	d(new Data)
{
	{
		auto layout = new QVBoxLayout();
		
		layout->addWidget(new QLabel(tr("<b>FORMAT</b>")));
		
		{
			auto cbox = new QComboBox();
			layout->addWidget(cbox);
			d->comboBox = cbox;
		}
		
		QPixmap transparentPixmap(16, 16);
		transparentPixmap.fill(Qt::transparent);
		
		QIcon enabledIcon;
		enabledIcon.addPixmap(QPixmap(":/icons/16x16/enabled.svg"), QIcon::Disabled, QIcon::On);
		//enabledIcon.addPixmap(QPixmap(":/icons/16x16/disabled.svg"), QIcon::Disabled, QIcon::Off);
		enabledIcon.addPixmap(transparentPixmap, QIcon::Disabled, QIcon::Off);
		
		layout->addWidget(new QLabel(tr("<b>FEATURES</b>")));
		
		for (auto capability : FormatSupport::allCapabilities())
		{
			auto hl = new QHBoxLayout();
			
			auto b = new SimpleButton(enabledIcon);
			b->setPressable(false);
			b->setCheckable(true);
			hl->addWidget(b);
			d->capabilityButtons[capability] = b;
			
			auto l = new QLabel();
			hl->addWidget(l);
			d->capabilityLabels[capability] = l;
			
			layout->addLayout(hl);
		}
		
		layout->addWidget(new QLabel(tr("<b>OPTIONS</b>")));
		
		{
			d->optionWidgetIndex = layout->count();
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
		
		auto widget = format->createExportingOptionWidget();
		
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
		d->layout->insertWidget(d->optionWidgetIndex, settingsWidget);
		settingsWidget->setVisible(true);
	}
	
	auto descriptionForCapability = [&](FormatSupport::Capability capability) -> QString
	{
		switch (capability)
		{
			case FormatSupport::CapabilityAlphaChannel:
				return tr("Alpha Channel");
			case FormatSupport::CapabilityLayers:
				return tr("Layers");
			case FormatSupport::CapabilityLossless:
				return tr("Lossless");
			default:
				return QString();
		}
	};
	
	auto supportDescription = [&](bool supported) -> QString
	{
		if (supported)
			return tr("Supported");
		else
			return tr("Not supported");
	};
	
	for (auto capability : FormatSupport::allCapabilities())
	{
		bool supported = format->capabilities() & capability;
		auto description = descriptionForCapability(capability) + " - " + supportDescription(supported);
		
		d->capabilityButtons[capability]->setChecked(supported);
		d->capabilityLabels[capability]->setText(description);
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
