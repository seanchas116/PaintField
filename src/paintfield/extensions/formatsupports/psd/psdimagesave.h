#pragma once

#include <Malachite/Image>
#include "psdlayerandmaskinformationsection.h"

namespace PaintField {
namespace PsdImageSave {

// data is RGBA order
void save(Malachite::Image &&image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos, int bpp);
void saveEmpty(QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos);

QByteArray saveAsImageData(Malachite::Image &&image, int bpp);

} // namespace PsdImageSave
} // namespace PaintField
