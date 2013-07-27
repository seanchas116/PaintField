#include <QTest>
#include <QDebug>
#include <Malachite/BlendMode>
#include <random>
#include <boost/range.hpp>

#include "test.h"

using namespace Malachite;

Test::Test(QObject *parent) :
	QObject(parent)
{
}

void Test::test_blend()
{
	constexpr int pixelCount = 10;
	
	std::random_device randomDevice;
	std::mt19937 randomEngine(randomDevice());
	std::uniform_real_distribution<float> unitDist(0.f, 1.f);
	
	auto makeRandomOpacity = [&]()
	{
		return unitDist(randomEngine);
	};
	
	auto makeRandomPixel = [&]()
	{
		float a = makeRandomOpacity();
		float r = makeRandomOpacity() * a;
		float g = makeRandomOpacity() * a;
		float b = makeRandomOpacity() * a;
		return Pixel(a, r, g, b);
	};
	
	auto makeRandomOpacities = [&]()
	{
		QVector<float> vec(pixelCount);
		
		for (int i = 0; i < pixelCount; ++i)
			vec[i] = makeRandomOpacity();
		
		return vec;
	};
	
	auto makeRandomPixels = [&]()
	{
		QVector<Pixel> vec(pixelCount);
		
		for (int i = 0; i < pixelCount; ++i)
			vec[i] = makeRandomPixel();
		
		return vec;
	};
	
	auto comparePixelVectors = [](const QVector<Pixel> &v1, const QVector<Pixel> &v2)
	{
		auto predicate = [](const Pixel &p1, const Pixel &p2)
		{
			return qFuzzyCompare(p1.a(), p2.a()) && qFuzzyCompare(p1.r(), p2.r()) && qFuzzyCompare(p1.g(), p2.g()) && qFuzzyCompare(p1.b(), p2.b());
		};
		
		return boost::range::equal(v1, v2, predicate);
	};
	
	auto verifyPixels = [&](const QVector<Pixel> &correct, const QVector<Pixel> &result)
	{
		auto compare = comparePixelVectors(correct, result);
		
		if (!compare)
		{
			qDebug() << "correct:";
			qDebug() << correct;
			qDebug() << "result:";
			qDebug() << result;
		}
		
		return compare;
	};
	
	auto correctBlend = [](const Pixel &dst, const Pixel &src)
	{
		return src.v() + (1.0f - src.a()) * dst.v();
	};
	
	auto blendOp = BlendMode(BlendMode::SourceOver).op();
	
	const auto dstPixels = makeRandomPixels();
	const auto srcPixels = makeRandomPixels();
	
	qDebug() << "dst:";
	qDebug() << dstPixels;
	qDebug() << "src:";
	qDebug() << srcPixels;
	
	// simple blending
	{
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
			correctResultPixels[i] = correctBlend(correctResultPixels[i], srcPixels[i]);
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blend(pixelCount, dst, src);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with pixel masks
	{
		const auto maskPixels = makeRandomPixels();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[i].v() * maskPixels[i].aV();
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(maskPixels.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with float masks
	{
		const auto masks = makeRandomOpacities();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[i].v() * masks[i];
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(masks.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with one pixel mask
	{
		const auto mask = makeRandomPixel();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[i].v() * mask.aV();
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with one float mask
	{
		const auto opacity = makeRandomOpacity();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[i].v() * opacity;
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src, opacity);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending one pixel
	{
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			correctResultPixels[i] = correctBlend(correctResultPixels[i], srcPixels[0]);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src[0]);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending one pixel with pixel masks
	{
		const auto maskPixels = makeRandomPixels();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[0].v() * maskPixels[i].aV();
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(maskPixels.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src[0], mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending one pixel with float masks
	{
		const auto masks = makeRandomOpacities();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[0].v() * masks[i];
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(masks.data(), pixelCount);
		
		blendOp->blend(srcPixels.size(), dst, src[0], mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// reverse blending
	
	// simple blending
	{
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
			correctResultPixels[i] = correctBlend(correctResultPixels[i], srcPixels[pixelCount - 1 - i]);
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blendReversed(srcPixels.size(), dst, src);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with pixel masks
	{
		const auto maskPixels = makeRandomPixels();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[pixelCount - 1 - i].v() * maskPixels[i].aV();
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(maskPixels.data(), pixelCount);
		
		blendOp->blendReversed(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with float masks
	{
		const auto masks = makeRandomOpacities();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[pixelCount - 1 - i].v() * masks[i];
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		auto mask = wrapPointer(masks.data(), pixelCount);
		
		blendOp->blendReversed(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with one pixel mask
	{
		const auto mask = makeRandomPixel();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[pixelCount - 1 - i].v() * mask.aV();
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blendReversed(srcPixels.size(), dst, src, mask);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
	
	// blending with one float mask
	{
		const auto opacity = makeRandomOpacity();
		
		auto correctResultPixels = dstPixels;
		
		for (int i = 0; i < pixelCount; ++i)
		{
			PixelVec src = srcPixels[pixelCount - 1 - i].v() * opacity;
			correctResultPixels[i] = correctBlend(correctResultPixels[i], src);
		}
		
		auto resultPixels = dstPixels;
		auto dst = wrapPointer(resultPixels.data(), pixelCount);
		auto src = wrapPointer(srcPixels.data(), pixelCount);
		
		blendOp->blendReversed(srcPixels.size(), dst, src, opacity);
		
		QVERIFY(verifyPixels(correctResultPixels, resultPixels));
	}
}

QTEST_MAIN(Test)
