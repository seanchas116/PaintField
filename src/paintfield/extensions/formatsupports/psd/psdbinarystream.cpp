#include "psdbinarystream.h"

namespace PaintField {

QByteArray PsdBinaryStream::read(qint64 size)
{
	auto data = m_device->read(size);
	if (data.size() != size)
		throw std::runtime_error("cannot read data anymore");
	return data;
}

void PsdBinaryStream::write(const QByteArray &data)
{
	auto size = m_device->write(data);
	if (size != data.size())
		throw std::runtime_error("cannot write data");
}

} // namespace PaintField
