#pragma once

#include "global.h"
#include <QVariant>
#include <QByteArray>

namespace PaintField
{

namespace Json
{

QVariant read(const QByteArray &data);
QVariant readFromFile(const QString &filepath);

QByteArray write(const QVariant &variant);
bool writeIntoFile(const QString &filepath, const QVariant &variant);

}

}
