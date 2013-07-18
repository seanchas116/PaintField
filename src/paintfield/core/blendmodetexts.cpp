#include "blendmodetexts.h"

using namespace Malachite;

namespace PaintField {

BlendModeTexts::BlendModeTexts(QObject *parent) :
	QObject(parent)
{
}

struct BlendModeTexts::Dictionary
{
	Dictionary()
	{
		texts[BlendMode::Normal] = tr("Normal");
		texts[BlendMode::Plus] = tr("Plus");
		texts[BlendMode::Multiply] = tr("Multiply");
		texts[BlendMode::Screen] = tr("Screen");
		texts[BlendMode::Overlay] = tr("Overlay");
		texts[BlendMode::Darken] = tr("Darken");
		texts[BlendMode::Lighten] = tr("Lighten");
		texts[BlendMode::ColorDodge] = tr("Color Dodge");
		texts[BlendMode::ColorBurn] = tr("Color Burn");
		texts[BlendMode::HardLight] = tr("Hard Light");
		texts[BlendMode::SoftLight] = tr("Soft Light");
		texts[BlendMode::Difference] = tr("Difference");
		texts[BlendMode::Exclusion] = tr("Exclusion");
		texts[BlendMode::Hue] = tr("Hue");
		texts[BlendMode::Saturation] = tr("Saturation");
		texts[BlendMode::Color] = tr("Color");
		texts[BlendMode::Luminosity] = tr("Luminosity");
		
		texts[BlendMode::PassThrough] = tr("Pass Through");
	}
	
	QHash<int, QString> texts;
};

BlendModeTexts::Dictionary *BlendModeTexts::dict = new BlendModeTexts::Dictionary();

QString BlendModeTexts::text(BlendMode mode)
{
	return dict->texts[mode.toInt()];
}

} // namespace PaintField
