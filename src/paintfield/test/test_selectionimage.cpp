#include "autotest.h"
#include "test_selectionimage.h"

#include "paintfield/core/selectionsurface.h"

namespace PaintField {

Test_SelectionImage::Test_SelectionImage(QObject *parent) :
	QObject(parent)
{
}


void Test_SelectionImage::test_isBlank()
{
	SelectionImage image(QSize(16, 16));
	image.fill(false);
	QCOMPARE(image.isBlank(), true);
	image.setPixel(10, 10, true);
	QCOMPARE(image.isBlank(), false);
}

void Test_SelectionImage::test_iterator()
{
	SelectionImage image(QSize(16, 16));
	image.fill(false);
	auto i = image.scanline(2);
	i += 11;
	auto j = image.scanline(2);
	j += 11;
	QCOMPARE(i, j);
	*i = true;
	QCOMPARE(image.pixel(11, 2), true);
	auto ci = image.constScanline(2);
	ci += 11;
	QCOMPARE(bool(*ci), true);
	ci -= 4;
	QCOMPARE(bool(*ci), false);
	ci += 4;
	QCOMPARE(bool(*ci), true);
}

void Test_SelectionImage::test_toQImageARGBPremult()
{
	SelectionImage image(QSize(16, 16));
	image.fill(true);
	image.setPixel(10, 10, false);
	auto rgb0 = qRgb(0,10,20);
	auto rgb1 = qRgb(30,40,50);
	auto qimage = image.toQImageARGBPremult(rgb1, rgb0);
	QCOMPARE(qimage.format(), QImage::Format_ARGB32_Premultiplied);
	QCOMPARE(qimage.size(), image.size());
	QCOMPARE(qimage.pixel(1,1), rgb1);
	QCOMPARE(qimage.pixel(10,10), rgb0);
}

PF_ADD_TESTCLASS(Test_SelectionImage)

}
