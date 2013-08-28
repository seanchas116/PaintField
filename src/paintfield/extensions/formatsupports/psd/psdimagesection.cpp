#include "psdbinarystream.h"

#include "psdimagesection.h"

namespace PaintField {

void PsdImageSection::save(PsdBinaryStream &stream)
{
	stream << uint16_t(0);
	stream.write(data);
}

} // namespace PaintField
