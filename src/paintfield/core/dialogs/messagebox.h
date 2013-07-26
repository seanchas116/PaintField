#pragma once

#include <QMessageBox>

namespace PaintField
{
namespace MessageBox
{

int show(QMessageBox::Icon iconType, const QString &text, const QString &informativeText, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

}
}

