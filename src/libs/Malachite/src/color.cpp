#include <stdexcept>

#include "misc.h"
#include "color.h"

using namespace std;

namespace Malachite
{

double Color::component(Component component) const
{
	switch (component)
	{
	case Red:
		return red();
	case Green:
		return green();
	case Blue:
		return blue();
	case Hue:
		return hue();
	case Saturation:
		return saturation();
	case Value:
		return value();
	default:
		return 0;
	}
}

void Color::setComponent(Component component, double x)
{
	switch (component)
	{
	case Red:
		setRed(x);
		break;
	case Green:
		setGreen(x);
		break;
	case Blue:
		setBlue(x);
		break;
	case Hue:
		setHue(x);
		break;
	case Saturation:
		setSaturation(x);
		break;
	case Value:
		setValue(x);
		break;
	default:
		break;
	}
}

void Color::rgbChanged()
{
	double max = max3(_r, _g, _b);
	double min = min3(_r, _g, _b);
	
	_v = max;
	double d = max - min;
	
	if (max) {
		_s = d / max;
	}
	
	if (max == min) {
		return;
	}
	
	double hDegrees;
	
	if (max == _r) {
		hDegrees = 60.0 * (_g - _b) / d;
	} else if (max == _g) {
		hDegrees = 60.0 * (_b - _r) / d + 120.0;
	} else {
		hDegrees = 60.0 * (_r - _g) / d + 240.0;
	}
	
	_h = hDegrees / 360.0;
	_h = _h - floor(_h);
}

void Color::hsvChanged()
{
	double d = _h * 6.0;
	int i = floor(d);
	double f = d - i;
	
	switch (i) {
	case 0:
		_r = _v;
		_g = _v * (1 - (1 - f) * _s);
		_b = _v * (1 - _s);
		break;
	case 1:
		_r = _v * (1 - f * _s);
		_g = _v;
		_b = _v * (1 - _s);
		break;
	case 2:
		_r = _v * (1 - _s);
		_g = _v;
		_b = _v * (1 - (1 - f) * _s);
		break;
	case 3:
		_r = _v * (1 - _s);
		_g = _v * (1 - f * _s);
		_b = _v;
		break;
	case 4:
		_r = _v * (1 - (1 - f) * _s);
		_g = _v * (1 - _s);
		_b = _v;
		break;
	case 5:
		_r = _v;
		_g = _v * (1 - _s);
		_b = _v * (1 - f * _s);
		break;
	default:
		break;
	}
}

QString Color::toWebColor() const
{
	Color color = *this;
	color.setAlpha(1.0);
	BgraU8 pix8 = color.toPixel();
	
	QString rText = QString::number(pix8.r(), 16).toUpper();
	if (rText.size() == 1)
		rText = "0" + rText;
	
	QString gText = QString::number(pix8.g(), 16).toUpper();
	if (gText.size() == 1)
		gText = "0" + gText;
	
	QString bText = QString::number(pix8.b(), 16).toUpper();
	if (bText.size() == 1)
		bText = "0" + bText;
	
	return "#" + rText + gText + bText;
}

Color Color::fromWebColor(const QString &webColor)
{
	int size = webColor.size();
	if (size != 4 && size != 7)
		throw runtime_error("wrong string length");
	
	if (webColor.at(0) != '#')
		throw runtime_error("no #");
	
	BgraU8 pix8;
	bool textOk;
	
	uint32_t value = webColor.mid(1).toULong(&textOk, 16);
	if (!textOk)
		throw runtime_error("cannot convert to hex");
	
	pix8.ra() = 0xFF;
	
	if (size == 7)
	{
		pix8.rr() = (value >> 16) & 0xFF;
		pix8.rg() = (value >> 8) & 0xFF;
		pix8.rb() = value & 0xFF;
	}
	else if (size == 4)
	{
		int r = (value >> 8) & 0xF;
		pix8.rr() = r | (r << 4);
		
		int g = (value >> 4) & 0xF;
		pix8.rg() = g | (g << 4);
		
		int b = value & 0xF;
		pix8.rb() = b | (b << 4);
	}
	
	return Color::fromPixel(pix8);
}

QDataStream &operator<<(QDataStream &out, const Color &color)
{
	out << color._a << color._r << color._g << color._b << color._h << color._s << color._v;
	return out;
}

QDataStream &operator>>(QDataStream &in, Color &color)
{
	in >> color._a >> color._r >> color._g >> color._b >> color._h >> color._s >> color._v;
	return in;
}

}

