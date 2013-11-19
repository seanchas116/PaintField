#pragma once

//ExportName: Misc

#include <QVector>
#include <QSet>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QTransform>
#include <cmath>
#include <cstring>
#include "global.h"

#include <cstdlib>
#ifdef __MINGW32__
#include <malloc.h>
#endif

typedef QList<QPoint>	QPointList;
typedef QSet<QPoint>	QPointSet;

namespace Malachite
{

#define ML_CURRENT_FUNCTION	__PRETTY_FUNCTION__

template <typename T>
inline T max3(const T &a, const T &b, const T &c)
{
	if (a > b && a > c) {
		return a;
	} else {
		if (b > c)
			return b;
		else
			return c;
	}
}

template <typename T>
inline T min3(const T &a, const T &b, const T &c)
{
	if (a < b && a < c) {
		return a;
	} else {
		if (b < c)
			return b;
		else
			return c;
	}
}

/**
  Returns the bit-to-bit reinterpretation of src.

  Example:
  double value = 3.14;
  uint64_t data = mlTransferCast<uint64_t>(value);
*/
template <class TypeDst, class TypeSrc>
inline TypeDst &blindCast(TypeSrc &src)
{
	return *reinterpret_cast<TypeDst *>(&src);
}

template <class TypeDst, class TypeSrc>
inline const TypeDst &blindCast(const TypeSrc &src)
{
	return *reinterpret_cast<const TypeDst *>(&src);
}

inline bool isTransformIntegerTranslating(const QTransform &transform)
{
	if (transform.isIdentity())
		return true;
	
	return transform.type() <= QTransform::TxTranslate && transform.dx() == floor(transform.dx()) && transform.dy() == floor(transform.dy());
}

inline bool isTransformSimilar(const QTransform &transform)
{
	return transform.isIdentity() || (transform.isAffine() && transform.m12() == 0 && transform.m21() == 0 && transform.m11() == transform.m22());
}

}

inline uint qHash(const QSize &key)
{
	QPair<int, int> pair(key.width(), key.height());
	return qHash(pair);
}

inline uint qHash(const QPoint &key)
{
	QPair<int, int> pair(key.x(), key.y());
	return qHash(pair);
}

