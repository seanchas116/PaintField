#pragma once

#include "paintfield/core/global.h"
#include <Malachite/Image>
#include "psdlayerandmaskinformationsection.h"

namespace PaintField {

namespace PsdImageLoad
{

Malachite::Image load(const QVector<Ref<PsdChannelData>> &channeldDataList, const QVector<PsdChannelInfo> &channelInfos, const QRect &rect, int bpp);

}

} // namespace PaintField

