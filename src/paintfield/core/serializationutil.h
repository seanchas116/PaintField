#pragma once

#include <QVariant>
#include <Malachite/Brush>
#include <QFont>

namespace PaintField {

namespace SerializationUtil {

QVariantMap mapFromColor(const Malachite::Color &c);
Malachite::Color colorFromMap(const QVariantMap &map);

QVariantMap mapFromBrush(const Malachite::Brush &brush);
Malachite::Brush brushFromMap(const QVariantMap &map);

QVariantMap mapFromFont(const QFont &font);
QFont fontFromMap(const QVariantMap &map);

QVariantMap mapFromAlignment(Qt::Alignment alignment);
Qt::Alignment alignmentFronMap(const QVariantMap &map);

}

} // namespace PaintField
