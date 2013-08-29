#include "psdbinarystream.h"

#include "psdfileheadersection.h"

namespace PaintField {

void PsdFileHeaderSection::load(PsdBinaryStream &stream)
{
	auto signature = stream.read(4);

	if (signature != "8BPS")
		throw std::runtime_error("not a PSD file");

	int16_t version;
	stream >> version;

	if (version != 1)
		throw std::runtime_error("version error");

	stream.move(6);
	stream >> channelCount;
	stream >> height;
	stream >> width;
	stream >> depth;
	stream >> colorMode;

	PAINTFIELD_DEBUG << "channel count" << channelCount;
	PAINTFIELD_DEBUG << "height" << height;
	PAINTFIELD_DEBUG << "width" << width;
	PAINTFIELD_DEBUG << "depth" << depth;
	PAINTFIELD_DEBUG << "color mode" << colorMode;
}

void PsdFileHeaderSection::save(PsdBinaryStream &stream) const
{
	stream.write("8BPS");
	stream << int16_t(1);
	stream.write(QByteArray(6, 0));
	stream << channelCount;
	stream << height;
	stream << width;
	stream << depth;
	stream << colorMode;
}

} // namespace PaintField
