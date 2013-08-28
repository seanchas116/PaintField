#pragma once

#include <Malachite/Image>
#include "psdlayerandmaskinformationsection.h"

namespace PaintField {
namespace PsdImageSave {

void save(Malachite::Image &&image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos);

} // namespace PsdImageSave
} // namespace PaintField
