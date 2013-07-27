#pragma once
#include <cmath>
#include "vector_sse.h"

namespace Malachite
{

namespace PixelParams
{

enum Premult { NoPremult, HasPremult };

enum Alpha { NoAlpha, HasAlpha };

struct IndexBGRA
{
	enum Index { B, G, R, A };
};

struct IndexRGBA
{
	enum Index { R, G, B, A };
};

struct IndexBGR
{
	enum Index { B, G, R, A = 0 };
};

struct IndexRGB
{
	enum Index { R, G, B, A = 0 };
};

struct ChannelFloat;
struct ChannelU8;
struct ChannelU16;

struct ChannelFloat
{
	typedef float ValueType;
	
	static constexpr ValueType max() { return 1.f; }
	static constexpr ValueType min() { return 0.f; }
	
	ChannelFloat(ValueType value) : value(value) {}
	ChannelFloat(const ChannelFloat &other) = default;
	
	ChannelFloat(ChannelU8 other);
	ChannelFloat(ChannelU16 other);
	
	void premultiply(float a)
	{
		value *= a;
	}
	
	void unpremultiply(float a)
	{
		if (a)
			value /= a;
		else
			value = 0;
	}
	
	void removePremultipliedAlpha(float a)
	{
		value = value + 1.f - a;
	}
	
	void removeAlpha(float a)
	{
		value = (value - 1.f) * a + 1.f;
	}
	
	ValueType value;
};

struct ChannelU8
{
	typedef uint8_t ValueType;
	
	static constexpr ValueType max() { return 0xFF; }
	static constexpr ValueType min() { return 0; }
	
	ChannelU8(ValueType value) : value(value) {}
	ChannelU8(const ChannelU8 &other) = default;
	
	ChannelU8(ChannelFloat other)
	{
		value = std::round(other.value * float(max()));
	}
	
	ChannelU8(ChannelU16 other);
	
	ValueType value;
};

struct ChannelU16
{
	typedef uint16_t ValueType;
	
	static constexpr ValueType max() { return 0xFFFF; }
	static constexpr ValueType min() { return 0; }
	
	ChannelU16(ValueType value) : value(value) {}
	ChannelU16(const ChannelU16 &other) = default;
	
	ChannelU16(ChannelFloat other)
	{
		value = std::round(other.value * float(max()));
	}
	
	ChannelU16(ChannelU8 other)
	{
		value = (other.value << 8) | other.value;
	}
	
	ValueType value;
};

inline ChannelFloat::ChannelFloat(ChannelU8 other)
{
	value = float(other.value) / float(other.max());
}

inline ChannelFloat::ChannelFloat(ChannelU16 other)
{
	value = float(other.value) / float(other.max());
}

inline ChannelU8::ChannelU8(ChannelU16 other)
{
	value = other.value >> 8;
}

} // PixelParams

template <PixelParams::Premult T_Premult, PixelParams::Alpha T_Alpha, typename T_Index, typename T_Channel>
class RgbPixel
{
public:
	
	typedef T_Index Index;
	typedef T_Channel ChannelType;
	
	typedef typename ChannelType::ValueType ValueType;
	
	static constexpr bool isPremultEnabled() { return T_Premult == PixelParams::HasPremult; }
	static constexpr bool hasAlpha() { return T_Alpha == PixelParams::HasAlpha; }
	static constexpr size_t count() { return hasAlpha() ? 4 : 3; }
	
	typedef Vector< ValueType, count() > VectorType;
	
	RgbPixel() {}
	
	RgbPixel(const VectorType &v) : _v(v) {}
	
	RgbPixel(ValueType r, ValueType g, ValueType b, ValueType a)
	{
		setA(a); setR(r); setG(g); setB(b);
	}
	
	RgbPixel(ValueType x) : _v(x) {}
	
	RgbPixel(const RgbPixel<T_Premult, T_Alpha, T_Index, T_Channel> &other) : _v(other._v) {}
	
	template <PixelParams::Premult Other_Premult, PixelParams::Alpha Other_Alpha, typename Other_Index, typename Other_Channel>
	RgbPixel(const RgbPixel<Other_Premult, Other_Alpha, Other_Index, Other_Channel> &other)
	{
		if (other.hasAlpha())
		{
			typedef PixelParams::ChannelFloat DefaultChannel;
			//typedef DefaultChannel::ValueType DefaultType;
			
			DefaultChannel a = Other_Channel(other.a());
			DefaultChannel r = Other_Channel(other.r());
			DefaultChannel g = Other_Channel(other.g());
			DefaultChannel b = Other_Channel(other.b());
			
			if (hasAlpha())
			{
				if (isPremultEnabled() == false && other.isPremultEnabled() == true)
				{
					r.unpremultiply(a.value);
					g.unpremultiply(a.value);
					b.unpremultiply(a.value);
				}
				
				if (isPremultEnabled() == true && other.isPremultEnabled() == false)
				{
					r.premultiply(a.value);
					g.premultiply(a.value);
					b.premultiply(a.value);
				}
			}
			else
			{
				if (other.isPremultEnabled())
				{
					r.removePremultipliedAlpha(a.value);
					g.removePremultipliedAlpha(a.value);
					b.removePremultipliedAlpha(a.value);
				}
				else
				{
					r.removeAlpha(a.value);
					g.removeAlpha(a.value);
					b.removeAlpha(a.value);
				}
			}
			
			setA(ChannelType(a).value);
			setR(ChannelType(r).value);
			setG(ChannelType(g).value);
			setB(ChannelType(b).value);
		}
		else
		{
			setA(ChannelType::max());
			setR(ChannelType(Other_Channel(other.r())).value);
			setG(ChannelType(Other_Channel(other.g())).value);
			setB(ChannelType(Other_Channel(other.b())).value);
		}
	}
	
	ValueType a() const { return hasAlpha() ? _v[Index::A] : ChannelType::min(); }
	ValueType r() const { return _v[Index::R]; }
	ValueType g() const { return _v[Index::G]; }
	ValueType b() const { return _v[Index::B]; }
	
	void setA(ValueType x) { if (hasAlpha()) _v[Index::A] = x; }
	void setR(ValueType x) { _v[Index::R] = x; }
	void setG(ValueType x) { _v[Index::G] = x; }
	void setB(ValueType x) { _v[Index::B] = x; }
	
	ValueType &ra()
	{
		static_assert(hasAlpha(), "no alpha channel");
		return _v[Index::A];
	}
	
	ValueType &rr() { return _v[Index::R]; }
	ValueType &rg() { return _v[Index::G]; }
	ValueType &rb() { return _v[Index::B]; }
	
	VectorType v() const { return _v; }
	void setV(const VectorType &v) { _v = v; }
	VectorType &rv() { return _v; }
	
	bool operator==(const RgbPixel &other) const
	{
		return _v == other._v;
	}
	
	bool operator!=(const RgbPixel &other) const
	{
		return _v != other._v;
	}
	
protected:
	
	VectorType _v;
};

typedef RgbPixel<PixelParams::HasPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelFloat> BgraPremultF;
typedef RgbPixel<PixelParams::HasPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelU8> BgraPremultU8;
typedef RgbPixel<PixelParams::HasPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelU16> BgraPremultU16;

typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelFloat> BgraF;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelU8> BgraU8;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexBGRA, PixelParams::ChannelU16> BgraU16;

typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexRGBA, PixelParams::ChannelFloat> RgbaF;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexRGBA, PixelParams::ChannelU8> RgbaU8;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::HasAlpha, PixelParams::IndexRGBA, PixelParams::ChannelU16> RgbaU16;

typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexBGR, PixelParams::ChannelFloat> BgrF;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexBGR, PixelParams::ChannelU8> BgrU8;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexBGR, PixelParams::ChannelU16> BgrU16;

typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexRGB, PixelParams::ChannelFloat> RgbF;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexRGB, PixelParams::ChannelU8> RgbU8;
typedef RgbPixel<PixelParams::NoPremult, PixelParams::NoAlpha, PixelParams::IndexRGB, PixelParams::ChannelU16> RgbU16;

}

