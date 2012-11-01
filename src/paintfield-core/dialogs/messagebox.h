#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

namespace PaintField
{

int showMessageBox(QMessageBox::Icon iconType, const QString &text, const QString &informativeText, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

}

#endif // MESSAGEBOX_H
