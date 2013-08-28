#pragma once

#include "paintfield/core/global.h"
#include <stdexcept>
#include <array>
#include <QIODevice>
#include <QByteArray>
#include <QStack>
#include "paintfield/core/global.h"


class QIODevice;

namespace PaintField {

class PsdBinaryStream
{
public:

	PsdBinaryStream(QIODevice *device) :
		m_device(device)
	{}

	QByteArray read(qint64 size);

	template <size_t N>
	std::array<uint8_t, N> read()
	{
		std::array<uint8_t, N> array;
		auto data = read(N);
		std::memcpy(array.data(), data.data(), N);
		return array;
	}

	void write(const QByteArray &data);

	template <size_t N>
	void write(const std::array<uint8_t, N> &array)
	{
		write(QByteArray(reinterpret_cast<const char *>(array.data()), N));
	}

	QByteArray peek(qint64 size)
	{
		auto data = read(size);
		move(-size);
		return data;
	}

	qint64 pos() const
	{
		return m_device->pos();
	}

	void seek(qint64 pos)
	{
		m_device->seek(pos);
	}

	void move(qint64 offset)
	{
		seek(pos() + offset);
	}

	void pushPos()
	{
		m_posStack.push(pos());
	}

	void popPos()
	{
		if (m_posStack.size() == 0)
			throw std::runtime_error("cannot pop position stack");
		seek(m_posStack.pop());
	}

	template <typename T>
	void markOffset()
	{
		m_offsetMarkPos.push(this->pos());
		*this << T(0);
	}

	template <typename T>
	void writeOffset()
	{
		auto markPos = m_offsetMarkPos.pop();
		auto offset = this->pos() - markPos;
		this->pushPos();
		this->seek(markPos);
		*this << T(offset);
		this->popPos();
	}

	PsdBinaryStream &operator<<(uint8_t value)
	{
		std::array<uint8_t, 1> array;
		array[0] = value;
		write(array);
		return *this;
	}

	PsdBinaryStream &operator<<(uint16_t value)
	{
		std::array<uint8_t, 2> array;
		array[0] = value >> 8;
		array[1] = value;
		write(array);
		return *this;
	}

	PsdBinaryStream &operator<<(uint32_t value)
	{
		std::array<uint8_t, 4> array;
		array[0] = value >> 24;
		array[1] = value >> 16;
		array[2] = value >> 8;
		array[3] = value;
		write(array);
		return *this;
	}

	PsdBinaryStream &operator<<(int8_t value)
	{
		return operator<<(reinterpret_cast<uint8_t &>(value));
	}

	PsdBinaryStream &operator<<(int16_t value)
	{
		return operator<<(reinterpret_cast<uint16_t &>(value));
	}

	PsdBinaryStream &operator<<(int32_t value)
	{
		return operator<<(reinterpret_cast<uint32_t &>(value));
	}

	PsdBinaryStream &operator>>(uint8_t &value)
	{
		auto array = read<1>();
		value = array[0];
		return *this;
	}

	PsdBinaryStream &operator>>(uint16_t &value)
	{
		auto array = read<2>();
		value = array[0] << 8 | array[1];
		return *this;
	}

	PsdBinaryStream &operator>>(uint32_t &value)
	{
		auto array = read<4>();
		value = array[0] << 24 | array[1] << 16 | array[2] << 8 | array[3];
		return *this;
	}

	PsdBinaryStream &operator>>(int8_t &value)
	{
		return operator>>(reinterpret_cast<uint8_t &>(value));
	}

	PsdBinaryStream &operator>>(int16_t &value)
	{
		return operator>>(reinterpret_cast<uint16_t &>(value));
	}

	PsdBinaryStream &operator>>(int32_t &value)
	{
		return operator>>(reinterpret_cast<uint32_t &>(value));
	}

private:

	QIODevice *m_device;
	QStack<qint64> m_posStack;
	QStack<qint64> m_offsetMarkPos;
};

} // namespace PaintField
