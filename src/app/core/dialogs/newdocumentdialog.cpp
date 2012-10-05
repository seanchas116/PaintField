#include "newdocumentdialog.h"
#include "ui_newdocumentdialog.h"

namespace PaintField
{

NewDocumentDialog::NewDocumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaintField_NewDocumentDialog)
{
    ui->setupUi(this);
}

QSize NewDocumentDialog::documentSize() const
{
	return QSize(ui->spinBoxWidth->value(), ui->spinBoxHeight->value());
}

NewDocumentDialog::~NewDocumentDialog()
{
    delete ui;
}

}
