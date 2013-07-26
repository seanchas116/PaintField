#pragma once

#include <QWidget>

namespace PaintField {

namespace Ui {
class PngExportForm;
}

class PngExportForm : public QWidget
{
	Q_OBJECT
	
public:
	explicit PngExportForm(QWidget *parent = 0);
	~PngExportForm();
	
	bool isAlphaEnabled() const;
	
private:
	Ui::PngExportForm *ui;
};


} // namespace PaintField
