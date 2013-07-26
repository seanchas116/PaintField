#include "messagebox.h"

namespace PaintField
{
namespace MessageBox
{

int show(QMessageBox::Icon iconType, const QString &text, const QString &informativeText, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
	QMessageBox box;
	box.setIcon(iconType);
	box.setText(text);
	box.setInformativeText(informativeText);
	box.setStandardButtons(buttons);
	box.setDefaultButton(defaultButton);
	
	return box.exec();
}

}
}
