#include "psdbinarystream.h"

#include "psdutils.h"

namespace PaintField {

namespace PsdUtils
{

QString readPascalString(PsdBinaryStream &stream, int unit)
{
	uint8_t count;
	stream >> count;

	int totalCount = count + 1;
	int alignedCount = ((totalCount - 1) / unit + 1) * unit;
	int skipCount = alignedCount - totalCount;

	auto data = stream.read(count);
	stream.move(skipCount);
	return QString::fromUtf8(data.data(), data.size());
}

void writePascalString(PsdBinaryStream &stream, int unit, const QString &string)
{
	QByteArray data = string.toUtf8();
	if (data.size() > 255)
		data = data.mid(0, 255);

	uint8_t count = data.size();
	int totalCount = count + 1;
	int alignedCount = ((totalCount - 1) / unit + 1) * unit;
	int skipCount = alignedCount - totalCount;

	stream << count;
	stream.write(data);
	stream.move(skipCount);
}

QString readUnicodePascalString(PsdBinaryStream &stream)
{
	uint32_t count;
	stream >> count;

	QString string;
	string.reserve(count);

	for (uint32_t i = 0; i < count; ++i)
	{
		uint16_t c;
		stream >> c;
		string += QChar(c);
	}

	return string;
}

void writeUnicodePascalString(PsdBinaryStream &stream, const QString &string)
{
	uint32_t count = string.size();
	stream << count;

	for (const auto c : string)
		stream << c.unicode();
}

QByteArray decodePackBits(const QByteArray &src, int dstSize)
{
	QByteArray dst;
	dst.reserve(dstSize);

	int index = 0;
	while (index < src.size())
	{
		char count = src[index];
		index += 1;
		if (count >= 0)
		{
			int len = count + 1;
			dst += src.mid(index, len);
			index += len;
		}
		else
		{
			char c = src[index];
			int len = -count + 1;
			for (int i = 0; i < len; ++i)
				dst += c;
			index += 1;
		}
	}

	return dst;
}

static QHash<QByteArray, Malachite::BlendMode> makeHashToBlendMode()
{
	QHash<QByteArray, Malachite::BlendMode> hash;
	hash["pass"] = Malachite::BlendMode::PassThrough;
	hash["norm"] = Malachite::BlendMode::Normal;
	hash["dark"] = Malachite::BlendMode::Darken;
	hash["mul "] = Malachite::BlendMode::Multiply;
	hash["idiv"] = Malachite::BlendMode::ColorBurn;
	hash["lite"] = Malachite::BlendMode::Lighten;
	hash["scrn"] = Malachite::BlendMode::Screen;
	hash["div "] = Malachite::BlendMode::ColorDodge;
	hash["over"] = Malachite::BlendMode::Overlay;
	hash["sLit"] = Malachite::BlendMode::SoftLight;
	hash["hLit"] = Malachite::BlendMode::HardLight;
	hash["diff"] = Malachite::BlendMode::Difference;
	hash["smud"] = Malachite::BlendMode::Exclusion;
	hash["hue "] = Malachite::BlendMode::Hue;
	hash["sat "] = Malachite::BlendMode::Saturation;
	hash["lum "] = Malachite::BlendMode::Luminosity;
	return hash;
}

static auto hashToBlendMode = makeHashToBlendMode();

Malachite::BlendMode decodeBlendMode(const QByteArray &data)
{
	return hashToBlendMode.value(data, Malachite::BlendMode::Normal);
}

QByteArray encodeBlendMode(Malachite::BlendMode blendMode)
{
	return hashToBlendMode.key(blendMode, "norm");
}

}

} // namespace PaintField
