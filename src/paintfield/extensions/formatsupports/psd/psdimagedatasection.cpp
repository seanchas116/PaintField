#include "psdbinarystream.h"

#include "psdimagedatasection.h"

namespace PaintField {

void PsdImageDataSection::save(PsdBinaryStream &stream)
{
	stream << uint16_t(0);
	stream.write(data);
}

} // namespace PaintField
