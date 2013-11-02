#pragma once

#include <QRect>
#include <QByteArray>
#include "paintfield/core/global.h"

namespace PaintField {

class PsdBinaryStream;

class PsdChannelData
{
public:
	void load(PsdBinaryStream &stream, const QRect &rect, int depth, int dataSize);
	void save(PsdBinaryStream &stream) const;

	QByteArray rawData;
};

class PsdChannelInfo
{
public:

	void load(PsdBinaryStream &stream);
	void save(PsdBinaryStream &stream) const;

	int16_t id = 0;
	uint32_t length = 0;
};


class PsdLayerRecord
{
public:

	enum struct Flag
	{
		TransparencyProtected = 1 << 0,
		Visible = 1 << 1
	};

	enum struct SectionType
	{
		Other = 0,
		OpenFolder = 1,
		ClosedFolder = 2,
		BoundingSectionDivider = 3
	};

	QRect getRect() const
	{
		return QRect(rectLeft, rectTop, rectRight - rectLeft, rectBottom - rectTop);
	}

	void load(PsdBinaryStream &stream);
	void save(PsdBinaryStream &stream) const;

	int32_t rectTop = 0;
	int32_t rectLeft = 0;
	int32_t rectBottom = 0;
	int32_t rectRight = 0;

	QVector<PsdChannelInfo> channelInfos;

	QByteArray blendModeKey = "norm";

	uint8_t opacity = 255;
	uint8_t clipping = 0;
	uint8_t flags = 0;

	QString layerName;

	uint32_t sectionType = 0;
	QByteArray sectionBlendModeKey = "norm";

	QVector<SP<PsdChannelData>> channelDatas;
};

class PsdLayerInfo
{
public:

	void load(PsdBinaryStream &stream, int depth);
	void save(PsdBinaryStream &stream) const;

	QList<SP<PsdLayerRecord>> layerRecords;
};

class PsdLayerAndMaskInformationSection
{
public:

	PsdLayerInfo layerInfo;

	void load(PsdBinaryStream &stream, int depth);
	void save(PsdBinaryStream &stream) const;
};

} // namespace PaintField
