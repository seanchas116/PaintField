#include "brushstrokersimplebrush.h"

#include "brushrasterizer.h"
#include "custombrusheditor.h"

#include <Malachite/BlendTraits>
#include <Malachite/Interval>


namespace PaintField {

using namespace Malachite;

BrushStrokerSimpleBrush::BrushStrokerSimpleBrush(Surface *surface) :
    BrushStroker(surface)
{
}

void BrushStrokerSimpleBrush::drawFirst(const TabletInputData &data)
{
	int maxDabWidth = std::ceil(2 * radiusBase()) + 1;
	mCovers.reset(new PixelVec[maxDabWidth % 4 ? maxDabWidth / 4 + 1 : maxDabWidth / 4]);
	mCarryOver = 1;
	drawDab(data.pos, data.pressure);
}

void BrushStrokerSimpleBrush::drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd)
{
	int count = polygon.size() - 1;
	
	if (count < 1)
		return;
	
	if (count == 1)
	{
		auto p0 = polygon[0];
		auto p1 = polygon[1];
		auto len = (p1 - p0).length();
		auto pressure = dataStart.pressure;
		auto pressureNormalized = (dataEnd.pressure - dataStart.pressure) / len;
		mCarryOver = drawSegment(p0, p1, len, pressure, pressureNormalized, mCarryOver);
		return;
	}
	
	double totalLen;
	QVector<double> lengths = calcLength(polygon, &totalLen);
	
	double pressureNormalized = (dataEnd.pressure - dataStart.pressure) / totalLen;
	
	double pressure = dataStart.pressure;
	double carryOver = mCarryOver;
	
	for (int i = 0; i < count; ++i)
		carryOver = drawSegment(polygon[i], polygon[i+1], lengths[i], pressure, pressureNormalized, carryOver);
	
	mCarryOver = carryOver;
}

double BrushStrokerSimpleBrush::drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double len, double &pressure, double pressureNormalized, double carryOver)
{
	if (len == 0)
		return carryOver;
	
	if (carryOver > len)
		return carryOver - len;
	
	Vec2D posNormalized = (p2 - p1) / len;
	
	Vec2D pos = p1 + posNormalized * carryOver;
	pressure += pressureNormalized * carryOver;
	
	QRect firstRect = drawDab(pos, pressure);
	
	len -= carryOver;
	
	QRect lastRect;
	
	forever
	{
		len -= 1;
		if (len < 0)
			break;
		
		pos += posNormalized;
		pressure += pressureNormalized;
		
		lastRect = drawDab(pos, pressure);
	}
	
	QRect editedRect;
	
	if (lastRect.isEmpty())
		editedRect = firstRect;
	else
		editedRect = firstRect | lastRect;
	
	addEditedRect(editedRect);
	
	return -len;
}

inline static PixelVec sseVec4FromInt(int x1, int x2, int x3, int x4)
{
	union
	{
		std::array<int32_t, 4> a;
		__m128i m;
	} u;
	
	u.a[0] = x1;
	u.a[1] = x2;
	u.a[2] = x3;
	u.a[3] = x4;
	return _mm_cvtepi32_ps(u.m);
}

inline static PixelVec sseVec4FromInt(int32_t i)
{
	union
	{
		std::array<int32_t, 4> a;
		__m128i m;
	} u;
	
	u.a[0] = i;
	u.m = _mm_unpacklo_epi32(u.m, u.m);
	u.m = _mm_unpacklo_epi32(u.m, u.m);
	return _mm_cvtepi32_ps(u.m);
}

class BrushDab
{
public:
	BrushDab(const Vec2D &pos, double radius)
	{
		mPos = pos;
		mRect = QRectF(pos.x() - radius, pos.y() - radius, radius * 2.0, radius * 2.0).toAlignedRect();

		constexpr float aaWidth = 1.f;
		float max, cutoffSlope;

		if (radius <= 1.f)
		{
			max = radius;
			//cutoff = 0.f;
			radius = 1.f;
			cutoffSlope = -max;
		}
		else if (radius <= 1.f + aaWidth)
		{
			max = 1.f;
			//cutoff = 0.f;
			cutoffSlope = -max / radius;
		}
		else
		{
			max = 1.f;
			//cutoff = radius - aaWidth;
			cutoffSlope = -max / aaWidth;
		}

		mMaxs = PixelVec(max);
		mRadiuses = PixelVec(radius);
		mCutoffSlopes = PixelVec(cutoffSlope);
	}

	template <typename TOperation>
	void eachPixelInDab(BrushStrokerSimpleBrush *stroker, Surface *surface, TOperation func) const
	{
		constexpr auto tileWidth = Surface::tileWidth();
		auto tileTopLeft = QPoint( mRect.left() / tileWidth, mRect.top() / tileWidth );
		auto tileBottomRight = QPoint( mRect.right() / tileWidth, mRect.bottom() / tileWidth);

		for (int tileY = tileTopLeft.y(); tileY <= tileBottomRight.y(); ++tileY)
		{
			for (int tileX = tileTopLeft.x(); tileX <= tileBottomRight.x(); ++tileX)
			{
				QPoint key(tileX, tileY);

				auto image = stroker->getTile(key, surface);

				auto subRect = QRect(0, 0, tileWidth, tileWidth) & mRect.translated(-tileX * tileWidth, -tileY * tileWidth);

				auto x0 = subRect.left();
				auto y0 = subRect.top();

				auto offsetCenter = mPos - key * tileWidth - 0.5;

				PixelVec offsetCenterXs(offsetCenter.x());
				PixelVec offsetCenterYs(offsetCenter.y());

				auto xs0 = sseVec4FromInt(x0, x0+1, x0+2, x0+3) - offsetCenterXs;
				auto ys = sseVec4FromInt(y0) - offsetCenterYs;

				int w = subRect.width();

				for (int y = subRect.top(); y <= subRect.bottom(); ++y)
				{
					auto xs = xs0;
					auto yys = ys * ys;

					auto sl = image->pixelPointer(x0, y);

					int rem = w;

					while (rem)
					{
						auto rrs = xs * xs + yys;
						auto rs = rrs.rsqrt() * rrs;
						auto covers = ((rs - mRadiuses) * mCutoffSlopes).bound(0.f, mMaxs);
						covers = PixelVec::choose( PixelVec::equal(covers, covers) , covers, mMaxs);

						if (!rem--) break;
						func(*sl, covers[0]);
						++sl;

						if (!rem--) break;
						func(*sl, covers[1]);
						++sl;

						if (!rem--) break;
						func(*sl, covers[2]);
						++sl;

						if (!rem--) break;
						func(*sl, covers[3]);
						++sl;

						xs += 4.f;
					}

					ys += 1.f;
				}
			}
		}
	}

	QRect rect() const
	{
		return mRect;
	}

private:

	Vec2D mPos;
	QRect mRect;
	PixelVec mMaxs, mRadiuses, mCutoffSlopes;
};


template <typename TBlendTraits>
static QRect drawDabToSurface(BrushStrokerSimpleBrush *stroker, Surface *surface, const Pixel &color, const Vec2D &pos, double radius)
{
	auto rect = QRectF(pos.x() - radius, pos.y() - radius, radius * 2.0, radius * 2.0).toAlignedRect();
	
	float max, cutoffSlope;
	constexpr float aaWidth = 1.f;
	
	if (radius <= 1.f)
	{
		max = radius;
		//cutoff = 0.f;
		radius = 1.f;
		cutoffSlope = -max;
	}
	else if (radius <= 1.f + aaWidth)
	{
		max = 1.f;
		//cutoff = 0.f;
		cutoffSlope = -max / radius;
	}
	else
	{
		max = 1.f;
		//cutoff = radius - aaWidth;
		cutoffSlope = -max / aaWidth;
	}
	
	auto maxs = PixelVec(max);
	auto radiuses = PixelVec(radius);
	auto cutoffSlopes = PixelVec(cutoffSlope);
	
	constexpr auto tileWidth = Surface::tileWidth();
	auto tileTopLeft = QPoint( rect.left() / tileWidth, rect.top() / tileWidth );
	auto tileBottomRight = QPoint( rect.right() / tileWidth, rect.bottom() / tileWidth);
	
	for (int tileY = tileTopLeft.y(); tileY <= tileBottomRight.y(); ++tileY)
	{
		for (int tileX = tileTopLeft.x(); tileX <= tileBottomRight.x(); ++tileX)
		{
			QPoint key(tileX, tileY);
			
			auto image = stroker->getTile(key, surface);
			
			auto subRect = QRect(0, 0, tileWidth, tileWidth) & rect.translated(-tileX * tileWidth, -tileY * tileWidth);
			
			auto x0 = subRect.left();
			auto y0 = subRect.top();
			
			auto offsetCenter = pos - key * tileWidth - 0.5;
			
			PixelVec offsetCenterXs(offsetCenter.x());
			PixelVec offsetCenterYs(offsetCenter.y());
			
			auto xs0 = sseVec4FromInt(x0, x0+1, x0+2, x0+3) - offsetCenterXs;
			auto ys = sseVec4FromInt(y0) - offsetCenterYs;
			
			int w = subRect.width();
			
			for (int y = subRect.top(); y <= subRect.bottom(); ++y)
			{
				auto xs = xs0;
				auto yys = ys * ys;
				
				auto sl = image->pixelPointer(x0, y);
				
				int rem = w;
				
				while (rem)
				{
					auto rrs = xs * xs + yys;
					auto rs = rrs.rsqrt() * rrs;
					auto covers = ((rs - radiuses) * cutoffSlopes).bound(0.f, maxs);
					covers = PixelVec::choose( PixelVec::equal(covers, covers) , covers, maxs);
					
					if (!rem--) break;
					*sl = TBlendTraits::blend(*sl, color * covers[0]);
					++sl;
					
					if (!rem--) break;
					*sl = TBlendTraits::blend(*sl, color * covers[1]);
					++sl;
					
					if (!rem--) break;
					*sl = TBlendTraits::blend(*sl, color * covers[2]);
					++sl;
					
					if (!rem--) break;
					*sl = TBlendTraits::blend(*sl, color * covers[3]);
					++sl;
					
					xs += 4.f;
				}
				
				ys += 1.f;
			}
		}
	}
	
	return rect;
}

QRect BrushStrokerSimpleBrush::drawDab(const Vec2D &pos, double pressure)
{
	if (pressure <= 0)
		return QRect();
	
	auto radius = radiusBase() * pressure;
	auto color = pixel();

	BrushDab dab(pos, radius);

	if (mSmudge) {

		PixelVec smudgeColor(0);
		float smudgeDivisor = 0.f;
		dab.eachPixelInDab(this, surface(), [&](const Pixel &p, float cover) {
			smudgeColor += p;
			smudgeDivisor += cover;
		});
		smudgeColor /= smudgeDivisor;

		PAINTFIELD_DEBUG << smudgeColor.at(0) << smudgeColor.at(1) << smudgeColor.at(2) << smudgeColor.at(3);

		color = mSmudge * smudgeColor + color * (1.f - mSmudge);
	}

	dab.eachPixelInDab(this, surface(), [=](Pixel &p, float cover) {
		p = BlendTraitsSourceOver::blend(p, color * cover);
	});

	return dab.rect();
}

static void drawScanlineInTile(Image *tileImage, const QPoint &offset, const BrushScanline &scanline, const Pixel &argb, BlendOp *blendOp)
{
	QPoint pos = scanline.pos - offset;
	
	Q_ASSERT(0 <= pos.y() && pos.y() < Surface::tileWidth());
	
	int count = scanline.count;
	bool isSolid = count < 0;
	if (isSolid)
		count = -count;
	
	Interval interval = Interval(0, Surface::tileWidth()) & Interval(pos.x(), count);
	
	if (interval.isValid())
	{
		auto p = tileImage->pixelPointer(interval.start(), pos.y());
		
		if (isSolid)
		{
			blendOp->blend(count, p, argb);
		}
		else
		{
			auto pcover = scanline.covers + (interval.start() - pos.x());
			blendOp->blend(interval.length(), p, argb, wrapPointer(scanline.covers, count * sizeof(pcover), pcover));
		}
	}
}


void BrushStrokerSimpleBrush::drawScanline(const BrushScanline &scanline, Surface *surface)
{
	int tileY = IntDivision(scanline.pos.y(), Surface::tileWidth()).quot();
	int tileStart = IntDivision(scanline.pos.x(), Surface::tileWidth()).quot();
	int tileEnd = IntDivision(scanline.pos.x() + scanline.count - 1, Surface::tileWidth()).quot();
	
	auto blendOp = BlendMode(BlendMode::SourceOver).op();
	
	for (int tileX = tileStart; tileX <= tileEnd; ++tileX)
	{
		QPoint key(tileX, tileY);
		
		auto tile = getTile(key, surface);
		drawScanlineInTile(tile, key * Surface::tileWidth(), scanline, pixel(), blendOp);
	}
}

Image *BrushStrokerSimpleBrush::getTile(const QPoint &key, Surface *surface)
{
	if (mLastTile && mLastKey == key)
	{
		return mLastTile;
	}
	else
	{
		mLastKey = key;
		mLastTile = &surface->tileRef(key);
		return mLastTile;
	}
}

void BrushStrokerSimpleBrush::loadSettings(const QVariantMap &settings)
{
	mSmudge = settings.value("smudge", 0.0).toDouble();
}

BrushStrokerSimpleBrushFactory::BrushStrokerSimpleBrushFactory(QObject *parent) :
    BrushStrokerFactory(parent)
{
	
}

QString BrushStrokerSimpleBrushFactory::name() const
{
	return "paintfield.brush.simpleBrush";
}

QVariantMap BrushStrokerSimpleBrushFactory::defaultSettings() const
{
	return {
		{"smudge", 0.0}
	};
}

BrushStroker *BrushStrokerSimpleBrushFactory::createStroker(Surface *surface)
{
	return new BrushStrokerSimpleBrush(surface);
}

BrushEditor *BrushStrokerSimpleBrushFactory::createEditor(const QVariantMap &settings)
{
	auto editor = new CustomBrushEditor();
	editor->setSettings(settings);
	return editor;
}

} // namespace PaintField
