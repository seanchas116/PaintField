#pragma once

#include <QDialog>

namespace PaintField {

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit AboutDialog(QWidget *parent = 0);
	~AboutDialog();
	
private:
	Ui::AboutDialog *ui;
};


} // namespace PaintField
