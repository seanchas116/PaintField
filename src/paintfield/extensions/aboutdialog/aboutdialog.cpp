#include <QApplication>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

namespace PaintField {

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->versionLabel->setText("<h3>Version " + qApp->applicationVersion() + "</h3>");
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

} // namespace PaintField
