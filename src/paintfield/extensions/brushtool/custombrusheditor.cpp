#include "custombrusheditor.h"

#include "paintfield/core/widgets/looseslider.h"
#include "paintfield/core/widgets/loosespinbox.h"

#include <QHBoxLayout>
#include <QFormLayout>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>

#include <boost/signals2.hpp>

namespace PaintField {

struct CustomBrushEditor::Data
{
	QVariantMap mSettings;
	boost::signals2::signal<void(const QVariantMap &)> settingsChanged;
};

CustomBrushEditor::CustomBrushEditor(QWidget *parent) :
	BrushEditor(parent),
	d(new Data)
{
	{
		auto layout = new QFormLayout();

		auto addSliderItem = [&](const QString &key, const QString &text, double min, double max, int decimals) {

			auto hlayout = new QHBoxLayout();

			auto slider = new LooseSlider(Qt::Horizontal);
			slider->setRange(min, max);
			slider->setDecimals(decimals);

			hlayout->addWidget(slider);

			auto spinBox = new LooseSpinBox();
			spinBox->setRange(min, max);
			spinBox->setDecimals(decimals);

			connect(spinBox, &LooseSpinBox::valueChanged, slider, &LooseSlider::setDoubleValue);
			connect(slider, &LooseSlider::doubleValueChanged, spinBox, &LooseSpinBox::setValue);

			connect(spinBox, &LooseSpinBox::valueChanged, [=](double value) {
				d->mSettings[key] = value;
				emit settingsChanged(d->mSettings);
			});
			d->settingsChanged.connect([=](const QVariantMap &settings) {
				spinBox->setValue(settings[key].toDouble());
			});
			hlayout->addWidget(spinBox);

			layout->addRow(text, hlayout);
		};

		addSliderItem("erasing", tr("Erasing"), 0, 1, 2);
		addSliderItem("smudge", tr("Smudge"), 0, 1, 2);

		setLayout(layout);
	}
}

CustomBrushEditor::~CustomBrushEditor()
{
}

void CustomBrushEditor::setSettings(const QVariantMap &settings)
{
	d->mSettings = settings;
	d->settingsChanged(settings);
}

} // namespace PaintField
