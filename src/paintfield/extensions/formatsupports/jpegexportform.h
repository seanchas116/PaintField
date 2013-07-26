#pragma once

#include <QWidget>

namespace PaintField {

namespace Ui {
class JpegExportForm;
}

class JpegExportForm : public QWidget
{
	Q_OBJECT
	
public:
	explicit JpegExportForm(QWidget *parent = 0);
	~JpegExportForm();
	
	int quality() const;
	
private:
	Ui::JpegExportForm *ui;
};


} // namespace PaintField
