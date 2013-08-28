#pragma once

#include <QString>
#include <QByteArray>
#include <Malachite/BlendMode>
#include "paintfield/core/global.h"

namespace PaintField {

class PsdBinaryStream;

namespace PsdUtils
{

QString readPascalString(PsdBinaryStream &stream, int unit);
void writePascalString(PsdBinaryStream &stream, int unit, const QString &string);

QString readUnicodePascalString(PsdBinaryStream &stream);
void writeUnicodePascalString(PsdBinaryStream &stream, const QString &string);

QByteArray decodePackBits(const QByteArray &src, int dstSize);

Malachite::BlendMode decodeBlendMode(const QByteArray &data);
QByteArray encodeBlendMode(Malachite::BlendMode blendMode);

}

} // namespace PaintField

