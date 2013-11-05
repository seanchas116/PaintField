#include "custombrusheditor.h"

#include "paintfield/core/observablevariantmap.h"
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

CustomBrushEditor::CustomBrushEditor(ObservableVariantMap *parameters, QWidget *parent) :
	QWidget(parent),
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
			parameters->setValueOn<double>(key, spinBox, &LooseSpinBox::valueChanged);
			parameters->onValueChanged<double>(key, spinBox, &LooseSpinBox::setValue);

			hlayout->addWidget(spinBox);

			layout->addRow(text, hlayout);
		};

		addSliderItem("smudge", tr("Smudge"), 0, 1, 2);

		setLayout(layout);
	}
}

CustomBrushEditor::~CustomBrushEditor()
{
}

} // namespace PaintField
