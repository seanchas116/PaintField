#include "blendmode.h"

namespace Malachite
{

BlendMode::Dictionary::Dictionary()
{
	nameHash.reserve(27);
	qtCompositionModeHash.reserve(27);
	
	nameHash[Normal] = "normal";
	nameHash[Plus] = "plus";
	nameHash[Multiply] = "multiply";
	nameHash[Screen] = "screen";
	nameHash[Overlay] = "overlay";
	nameHash[Darken] = "darken";
	nameHash[Lighten] = "lighten";
	nameHash[ColorDodge] = "color-dodge";
	nameHash[ColorBurn] = "color-burn";
	nameHash[HardLight] = "hard-light";
	nameHash[SoftLight] = "soft-light";
	nameHash[Difference] = "difference";
	nameHash[Exclusion] = "exclusion";
	
	nameHash[Clear] = "clear";
	nameHash[Source] = "source";
	nameHash[Destination] = "destination";
	nameHash[SourceOver] = "source-over";
	nameHash[DestinationOver] = "destination-over";
	nameHash[SourceIn] = "source-in";
	nameHash[DestinationIn] = "destination-in";
	nameHash[SourceOut] = "source-out";
	nameHash[DestinationOut] = "destination-out";
	nameHash[SourceAtop] = "source-atop";
	nameHash[DestinationAtop] = "destination-atop";
	nameHash[Xor] = "xor";
	
	qtCompositionModeHash[Normal] = QPainter::CompositionMode_SourceOver;
	qtCompositionModeHash[Plus] = QPainter::CompositionMode_Plus;
	qtCompositionModeHash[Multiply] = QPainter::CompositionMode_Multiply;
	qtCompositionModeHash[Screen] = QPainter::CompositionMode_Screen;
	qtCompositionModeHash[Overlay] = QPainter::CompositionMode_Overlay;
	qtCompositionModeHash[Darken] = QPainter::CompositionMode_Darken;
	qtCompositionModeHash[Lighten] = QPainter::CompositionMode_Lighten;
	qtCompositionModeHash[ColorDodge] = QPainter::CompositionMode_ColorDodge;
	qtCompositionModeHash[ColorBurn] = QPainter::CompositionMode_ColorBurn;
	qtCompositionModeHash[HardLight] = QPainter::CompositionMode_HardLight;
	qtCompositionModeHash[SoftLight] = QPainter::CompositionMode_SoftLight;
	qtCompositionModeHash[Difference] = QPainter::CompositionMode_Difference;
	qtCompositionModeHash[Exclusion] = QPainter::CompositionMode_Exclusion;
	
	qtCompositionModeHash[Clear] = QPainter::CompositionMode_Clear;
	qtCompositionModeHash[Source] = QPainter::CompositionMode_Source;
	qtCompositionModeHash[Destination] = QPainter::CompositionMode_Destination;
	qtCompositionModeHash[SourceOver] = QPainter::CompositionMode_SourceOver;
	qtCompositionModeHash[DestinationOver] = QPainter::CompositionMode_DestinationOver;
	qtCompositionModeHash[SourceIn] = QPainter::CompositionMode_SourceIn;
	qtCompositionModeHash[DestinationIn] = QPainter::CompositionMode_DestinationIn;
	qtCompositionModeHash[SourceOut] = QPainter::CompositionMode_SourceOut;
	qtCompositionModeHash[DestinationOut] = QPainter::CompositionMode_DestinationOut;
	qtCompositionModeHash[SourceAtop] = QPainter::CompositionMode_SourceAtop;
	qtCompositionModeHash[DestinationAtop] = QPainter::CompositionMode_DestinationAtop;
	qtCompositionModeHash[Xor] = QPainter::CompositionMode_Xor;
}

BlendMode::Dictionary BlendMode::_dict;

}
