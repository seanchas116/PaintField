#include "psdbinarystream.h"

#include "psdfileheadersection.h"

namespace PaintField {

void PsdFileHeaderSection::load(PsdBinaryStream &stream)
{
	signature = stream.read(4);
	stream >> version;
	stream.move(6);
	stream >> channelCount;
	stream >> height;
	stream >> width;
	stream >> depth;
	stream >> colorMode;

	PAINTFIELD_DEBUG << "signature" << QString(signature);
	PAINTFIELD_DEBUG << "channel count" << channelCount;
	PAINTFIELD_DEBUG << "height" << height;
	PAINTFIELD_DEBUG << "width" << width;
	PAINTFIELD_DEBUG << "depth" << depth;
	PAINTFIELD_DEBUG << "color mode" << colorMode;
}

void PsdFileHeaderSection::save(PsdBinaryStream &stream) const
{
	stream.write(signature);
	stream << version;
	stream.write(QByteArray(6, 0));
	stream << channelCount;
	stream << height;
	stream << width;
	stream << depth;
	stream << colorMode;
}

} // namespace PaintField
