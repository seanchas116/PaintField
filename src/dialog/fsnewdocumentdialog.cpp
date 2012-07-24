#include "fsnewdocumentdialog.h"
#include "ui_fsnewdocumentdialog.h"

FSNewDocumentDialog::FSNewDocumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FSNewDocumentDialog)
{
    ui->setupUi(this);
}

QSize FSNewDocumentDialog::documentSize() const
{
	return QSize(ui->spinBoxWidth->value(), ui->spinBoxHeight->value());
}

FSNewDocumentDialog::~FSNewDocumentDialog()
{
    delete ui;
}
