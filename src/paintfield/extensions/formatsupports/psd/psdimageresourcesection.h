#pragma once

namespace PaintField {

class PsdBinaryStream;

class PsdImageResourceSection
{
public:

	void load(PsdBinaryStream &stream);
	void save(PsdBinaryStream &stream) const;
};

} // namespace PaintField

