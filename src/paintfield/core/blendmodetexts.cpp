#include "blendmodetexts.h"

using namespace Malachite;

namespace PaintField {

struct BlendModeTexts::Data
{
	QHash<int, QString> texts;
};

BlendModeTexts::BlendModeTexts(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->texts[BlendMode::Normal] = tr("Normal");
	d->texts[BlendMode::Plus] = tr("Plus");
	d->texts[BlendMode::Multiply] = tr("Multiply");
	d->texts[BlendMode::Screen] = tr("Screen");
	d->texts[BlendMode::Overlay] = tr("Overlay");
	d->texts[BlendMode::Darken] = tr("Darken");
	d->texts[BlendMode::Lighten] = tr("Lighten");
	d->texts[BlendMode::ColorDodge] = tr("Color Dodge");
	d->texts[BlendMode::ColorBurn] = tr("Color Burn");
	d->texts[BlendMode::HardLight] = tr("Hard Light");
	d->texts[BlendMode::SoftLight] = tr("Soft Light");
	d->texts[BlendMode::Difference] = tr("Difference");
	d->texts[BlendMode::Exclusion] = tr("Exclusion");
	d->texts[BlendMode::Hue] = tr("Hue");
	d->texts[BlendMode::Saturation] = tr("Saturation");
	d->texts[BlendMode::Color] = tr("Color");
	d->texts[BlendMode::Luminosity] = tr("Luminosity");
	
	d->texts[BlendMode::PassThrough] = tr("Pass Through");
}

BlendModeTexts::~BlendModeTexts()
{
	delete d;
}

QString BlendModeTexts::text(BlendMode mode)
{
	return d->texts.value(mode.toInt());
}

} // namespace PaintField
