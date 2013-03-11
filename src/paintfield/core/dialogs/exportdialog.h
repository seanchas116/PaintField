#ifndef FSEXPORTDIALOG_H
#define FSEXPORTDIALOG_H

#include <QDialog>
#include <QHash>
#include "global.h"

namespace Ui {
class PaintField_ExportDialog;
}

class QListWidgetItem;

namespace PaintField {

class WidgetGroup;

class ExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ExportDialog(QWidget *parent = 0);
	~ExportDialog();
	
	QString currentText() const;
	QString currentFormat() const { return _format; }
	int currentQuality() const { return _quality; }
	
private slots:
	
	void onFormatItemChanged(QListWidgetItem *item);
	void onQualityValueChanged(int quality);
	
private:
	Ui::PaintField_ExportDialog *ui;
	
	WidgetGroup *_qualityGroup;
	
	QHash<QListWidgetItem *, QString> _itemsToFormats;
	QString _format;
	int _quality;
};

}

#endif // FSEXPORTDIALOG_H
