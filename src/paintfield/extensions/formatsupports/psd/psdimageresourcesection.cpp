#include "psdbinarystream.h"
#include "psdutils.h"

#include "psdimageresourcesection.h"

namespace PaintField {


void PsdImageResourceSection::load(PsdBinaryStream &stream)
{
	uint32_t length;
	stream >> length;
	PAINTFIELD_DEBUG << "length" << length;

	auto beginPos = stream.pos();
	stream.pushPos();

	while (true)
	{
		if (stream.peek(4) != "8BIM" || stream.pos() - beginPos > length)
			break;

		stream.move(4);
		uint16_t id;
		stream >> id;
		PAINTFIELD_DEBUG << "id" << id;

		auto name = PsdUtils::readPascalString(stream, 2);

		PAINTFIELD_DEBUG << "name" << name;

		uint32_t size;
		stream >> size;
		PAINTFIELD_DEBUG << "size" << size;

		stream.pushPos();

		if (id == 1026)
		{
			for (int i = 0; i < size / 2; ++i)
			{
				uint16_t group;
				stream >> group;
				PAINTFIELD_DEBUG << "group" << group;
			}
		}

		stream.popPos();
		stream.move(size);
	}

	stream.popPos();
	stream.move(length);
}

void PsdImageResourceSection::save(PsdBinaryStream &stream) const
{
	uint32_t length = 0;
	stream << length;
}

} // namespace PaintField
