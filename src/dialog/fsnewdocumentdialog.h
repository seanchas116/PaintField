#ifndef FSNEWDOCUMENTDIALOG_H
#define FSNEWDOCUMENTDIALOG_H

#include <QDialog>

namespace Ui {
	class FSNewDocumentDialog;
}

class FSNewDocumentDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit FSNewDocumentDialog(QWidget *parent = 0);
	~FSNewDocumentDialog();
	
	QSize documentSize() const;
	
private:
	Ui::FSNewDocumentDialog *ui;
};

#endif // FSNEWDOCUMENTDIALOG_H
