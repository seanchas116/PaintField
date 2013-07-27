#pragma once

//ExportName: Color

#include <cstdlib>
#include <cmath>
#include <QColor>

#include "misc.h"
#include "pixel.h"
#include "genericimage.h"

namespace Malachite
{

/**
 * The Color represents a color with RGB and HSV values.
 * All components are equal or greater than 0.0 and equal or less than 1.0
 */
class MALACHITESHARED_EXPORT Color
{
public:
	
	enum Component
	{
		Red,
		Green,
		Blue,
		Hue,
		Saturation,
		Value
	};
	
	Color() :
		_a(0), _r(0), _g(0), _b(0), _h(0), _s(0), _v(0) {}
	
	double alpha() const { return _a; }
	double red() const { return _r; }
	double green() const { return _g; }
	double blue() const { return _b; }
	
	double hue() const { return _h; }
	double saturation() const { return _s; }
	double value() const { return _v; }
	
	double component(Component component) const;
	
	void setAlpha(double a)
	{
		_a = qBound(0.0, a, 1.0);
	}
	
	void setRed(double r)
	{
		_r = qBound(0.0, r, 1.0);
		rgbChanged();
	}
	
	void setGreen(double g)
	{
		_g = qBound(0.0, g, 1.0);
		rgbChanged();
	}
	
	void setBlue(double b)
	{
		_b = qBound(0.0, b, 1.0);
		rgbChanged();
	}
	
	void setHue(double h)
	{
		_h = h - floor(h);
		hsvChanged();
	}
	
	void setSaturation(double s)
	{
		_s = qBound(0.0, s, 1.0);
		hsvChanged();
	}
	
	void setValue(double v)
	{
		_v = qBound(0.0, v, 1.0);
		hsvChanged();
	}
	
	void setRgb(double r, double g, double b)
	{
		_r = qBound(0.0, r, 1.0);
		_g = qBound(0.0, g, 1.0);
		_b = qBound(0.0, b, 1.0);
		rgbChanged();
	}
	
	void setHsv(double h, double s, double v)
	{
		_h = h - floor(h);
		_s = qBound(0.0, s, 1.0);
		_v = qBound(0.0, v, 1.0);
		hsvChanged();
	}
	
	void setComponent(Component component, double x);
	
	static Color fromRgbValue(double r, double g, double b, double a = 1.0)
	{
		Color color;
		color.setAlpha(a);
		color.setRgb(r, g, b);
		color.rgbChanged();
		return color;
	}

	static Color fromHsvValue(double h, double s, double v, double a = 1.0)
	{
		Color color;
		color.setAlpha(a);
		color.setHsv(h, s, v);
		color.hsvChanged();
		return color;
	}
	
	Pixel toPixel() const
	{
		Pixel p;
		p.setA(_a);
		p.setR(_r * _a);
		p.setG(_g * _a);
		p.setB(_b * _a);
		return p;
	}
	
	QColor toQColor() const { return QColor::fromRgbF(_r, _g, _b, _a); }
	QString toWebColor() const;
	
	QRgb toQRgb() const
	{
		return blindCast<QRgb>(BgraU8(toPixel()));
	}
	
	QRgb toQRgbPremult() const
	{
		return blindCast<QRgb>(BgraPremultU8(toPixel()));
	}
	
	static Color transparent() { return Color::fromRgbValue(0, 0, 0, 0); }
	static Color white() { return Color::fromRgbValue(1, 1, 1, 1); }
	static Color black() { return Color::fromRgbValue(0, 0, 0, 1); }
	
	static Color fromPixel(const Pixel &p)
	{
		return p.a() ? Color::fromRgbValue(p.r() / p.a(), p.g() / p.a(), p.b() / p.a(), p.a()) : Color();
	}
	
	static Color fromQColor(const QColor &qcolor)
	{
		return Color::fromRgbValue(qcolor.redF(), qcolor.greenF(), qcolor.blueF(), qcolor.alphaF());
	}
	
	/**
	 * Creates a Color from a web color string like "#c0ffee" or "#abc".
	 * Throws a std::exception if failed.
	 * @param webColor
	 * @return 
	 */
	static Color fromWebColor(const QString &webColor);
	
	bool operator==(const Color &other) const
	{
		return	_a == other._a && 
				_r == other._r &&
				_g == other._g &&
				_b == other._b &&
				_h == other._h &&
				_s == other._s &&
				_v == other._v;
	}
	
	bool operator!=(const Color &other) const
	{
		return !operator==(other);
	}
	
	friend QDataStream &operator<<(QDataStream &out, const Color &color);
	friend QDataStream &operator>>(QDataStream &in, Color &color);
	
private:
	
	void rgbChanged();
	void hsvChanged();
	
	double _a, _r, _g, _b, _h, _s, _v;
};

MALACHITESHARED_EXPORT QDataStream &operator<<(QDataStream &out, const Color &color);
MALACHITESHARED_EXPORT QDataStream &operator>>(QDataStream &in, Color &color);

}

Q_DECLARE_METATYPE(Malachite::Color)

