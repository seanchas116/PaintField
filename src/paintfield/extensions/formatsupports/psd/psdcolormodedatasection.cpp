#include "psdbinarystream.h"

#include "psdcolormodedatasection.h"

namespace PaintField {

void PsdColorModeDataSection::load(PsdBinaryStream &stream)
{
	uint32_t length;
	stream >> length;
	stream.move(length);

	PAINTFIELD_DEBUG << "length" << length;
}

void PsdColorModeDataSection::save(PsdBinaryStream &stream) const
{
	uint32_t length = 0;
	stream << length;
}

} // namespace PaintField
