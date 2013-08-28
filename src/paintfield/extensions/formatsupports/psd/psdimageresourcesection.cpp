#include "psdbinarystream.h"

#include "psdimageresourcesection.h"

namespace PaintField {


void PsdImageResourceSection::load(PsdBinaryStream &stream)
{
	uint32_t length;
	stream >> length;
	stream.move(length);

	PAINTFIELD_DEBUG << "length" << length;
}

void PsdImageResourceSection::save(PsdBinaryStream &stream) const
{
	uint32_t length = 0;
	stream << length;
}

} // namespace PaintField
