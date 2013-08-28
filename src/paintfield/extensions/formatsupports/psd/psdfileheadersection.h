#pragma once

#include "paintfield/core/global.h"
#include <QByteArray>

namespace PaintField {

class PsdBinaryStream;

class PsdFileHeaderSection
{
public:

	void load(PsdBinaryStream &stream);
	void save(PsdBinaryStream &stream) const;

	QByteArray signature = "8BPS";
	int16_t version = 1;
	int16_t channelCount = 4;
	int32_t height = 1;
	int32_t width = 1;
	int16_t depth = 16;
	int16_t colorMode = 3;
};

} // namespace PaintField
