#include "blendmode.h"
#include "blendtraits.h"

#include "blendop.h"

// Pixel blend mode based on SVG compositing specification

namespace Malachite
{

BlendOpDictionary::BlendOpDictionary()
{
	_blendOps[BlendMode::Clear] = new TemplateBlendOp<BlendTraitsClear>;
	_blendOps[BlendMode::Source] = new TemplateBlendOp<BlendTraitsSource>;
	_blendOps[BlendMode::Destination] = new TemplateBlendOp<BlendTraitsDestination>;
	_blendOps[BlendMode::SourceOver] = new TemplateBlendOp<BlendTraitsSourceOver>;
	_blendOps[BlendMode::DestinationOver] = new TemplateBlendOp<BlendTraitsDestinationOver>;
	_blendOps[BlendMode::SourceIn] = new TemplateBlendOp<BlendTraitsSourceIn>;
	_blendOps[BlendMode::DestinationIn] = new TemplateBlendOp<BlendTraitsDestinationIn>;
	_blendOps[BlendMode::SourceOut] = new TemplateBlendOp<BlendTraitsSourceOut>;
	_blendOps[BlendMode::DestinationOut] = new TemplateBlendOp<BlendTraitsDestinationOut>;
	_blendOps[BlendMode::SourceAtop] = new TemplateBlendOp<BlendTraitsSourceAtop>;
	_blendOps[BlendMode::DestinationAtop] = new TemplateBlendOp<BlendTraitsDestinationAtop>;
	_blendOps[BlendMode::Xor] = new TemplateBlendOp<BlendTraitsXor>;
	
	_blendOps[BlendMode::Normal] = _blendOps[BlendMode::SourceOver];
	_blendOps[BlendMode::Plus] = new TemplateBlendOp<BlendTraitsPlus>;
	_blendOps[BlendMode::Multiply] = new TemplateBlendOp<BlendTraitsMultiply>;
	_blendOps[BlendMode::Screen] = new TemplateBlendOp<BlendTraitsScreen>;
	_blendOps[BlendMode::Overlay] = new TemplateBlendOp<BlendTraitsOverlay>;
	_blendOps[BlendMode::Darken] = new TemplateBlendOp<BlendTraitsDarken>;
	_blendOps[BlendMode::Lighten] = new TemplateBlendOp<BlendTraitsLighten>;
	_blendOps[BlendMode::ColorDodge] = new TemplateBlendOp<BlendTraitsColorDodge>;
	_blendOps[BlendMode::ColorBurn] = new TemplateBlendOp<BlendTraitsColorBurn>;
	_blendOps[BlendMode::HardLight] = new TemplateBlendOp<BlendTraitsHardLight>;
	_blendOps[BlendMode::SoftLight] = new TemplateBlendOp<BlendTraitsSoftLight>;
	_blendOps[BlendMode::Difference] = new TemplateBlendOp<BlendTraitsDifference>;
	_blendOps[BlendMode::Exclusion] = new TemplateBlendOp<BlendTraitsExclusion>;
	_blendOps[BlendMode::Hue] = new TemplateBlendOp<BlendTraitsHue>;
	_blendOps[BlendMode::Saturation] = new TemplateBlendOp<BlendTraitsSaturation>;
	_blendOps[BlendMode::Color] = new TemplateBlendOp<BlendTraitsColor>;
	_blendOps[BlendMode::Luminosity] = new TemplateBlendOp<BlendTraitsLuminosity>;
	
	_defaultBlendOp = _blendOps[BlendMode::SourceOver];
}

BlendOpDictionary _BlendOpDictionary;

BlendOpDictionary *blendOpDictionary()
{
	return &_BlendOpDictionary;
}

}

