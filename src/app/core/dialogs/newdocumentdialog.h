#ifndef FSNEWDOCUMENTDIALOG_H
#define FSNEWDOCUMENTDIALOG_H

#include <QDialog>

namespace Ui {
	class PaintField_NewDocumentDialog;
}

namespace PaintField
{

class NewDocumentDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit NewDocumentDialog(QWidget *parent = 0);
	~NewDocumentDialog();
	
	QSize documentSize() const;
	
private:
	Ui::PaintField_NewDocumentDialog *ui;
};

}

#endif // FSNEWDOCUMENTDIALOG_H
