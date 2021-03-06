#pragma once

#include <QByteArray>
#include "paintfield/core/global.h"

namespace PaintField {

class PsdBinaryStream;

class PsdImageDataSection
{
public:

	QByteArray data;

	void save(PsdBinaryStream &stream);

};

} // namespace PaintField

