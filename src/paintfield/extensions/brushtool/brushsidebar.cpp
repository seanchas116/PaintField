#include "brushsidebar.h"

#include "brushpresetmanager.h"
#include "paintfield/core/observablevariantmap.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QFormLayout>
#include <QCheckBox>

namespace PaintField {

BrushSideBar::BrushSideBar(BrushPresetManager *presetManager, QWidget *parent) :
    QWidget(parent)
{
	auto formLayout = new QFormLayout();
	
	auto presetLabel = new QLabel();
	formLayout->addRow(tr("Preset"), presetLabel);

	auto brushSize = presetManager->parameters()->value("size").toInt();
	{
		auto hlayout = new QHBoxLayout();
		
		auto slider = new QSlider(Qt::Horizontal);
		slider->setMaximum(200);
		slider->setMinimum(1);

		slider->setValue(brushSize);
		slider->setValue(brushSize);

		connect(slider, &QSlider::valueChanged, presetManager, [presetManager](int size){
			presetManager->parameters()->setValue("size", size);
		});
		connect(presetManager->parameters(), &ObservableVariantMap::valueChanged, slider, [slider](const QString &key, const QVariant &value){
			if (key == "size")
				slider->setValue(value.toInt());
		});

		hlayout->addWidget(slider, 0);

		auto spinBox = new QSpinBox;
		spinBox->setMaximum(200);
		spinBox->setMinimum(1);

		spinBox->setValue(brushSize);
		connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
		connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);

		hlayout->addWidget(spinBox, 0);
		
		formLayout->addRow(tr("Size"), hlayout);
	}
	
	{
		auto checkBox = new QCheckBox(tr("Smooth"));
		checkBox->setToolTip(tr("Check to paint beautifully, uncheck to paint faster"));
		checkBox->setChecked(presetManager->commonParameters()->value("smooth").toBool());

		connect(checkBox, &QCheckBox::toggled, presetManager, [presetManager](bool check){
			presetManager->commonParameters()->setValue("smooth", check);
		});
		connect(presetManager->commonParameters(), &ObservableVariantMap::valueChanged, checkBox, [checkBox](const QString &key, const QVariant &value){
			if (key == "smooth")
				checkBox->setChecked(value.toBool());
		});
		formLayout->addRow(checkBox);
	}
	
	setLayout(formLayout);

	auto setTitle = [this, presetLabel](const QString &rawTitle) {

		QString title;
		if (rawTitle.isEmpty())
			title = QString("[%1]").arg(tr("Not Selected"));
		else
			title = rawTitle;

		presetLabel->setText(QString("<b>%1</b>").arg(title));
	};

	connect(presetManager, &BrushPresetManager::titleChanged, this, setTitle);
	setTitle(presetManager->title());
}

} // namespace PaintField
