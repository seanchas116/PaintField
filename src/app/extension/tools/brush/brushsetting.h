#ifndef FSBRUSHSETTING_H
#define FSBRUSHSETTING_H

namespace PaintField
{

class BrushSetting
{
public:
	
	BrushSetting();
	
	double diameter;
	double diameterGamma;
	double flattening; // 扁平率
	double rotation;
	double tableWidth, tableHeight;
	double erasing;
	double smudge;
	double opacity;
};

}

#endif // FSBRUSHSETTING_H
