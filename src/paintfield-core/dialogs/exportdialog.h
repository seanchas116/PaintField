#ifndef FSEXPORTDIALOG_H
#define FSEXPORTDIALOG_H

#include <QDialog>
#include <QHash>

namespace Ui {
class PaintField_ExportDialog;
}

namespace PaintField {

class WidgetGroup;

class ExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ExportDialog(QWidget *parent = 0);
	~ExportDialog();
	
	//QString formatText(const QString &formatName) const { return _hash.value(formatName); }
	QString currentText() const;
	QString currentFormat() const { return _format; }
	int currentQuality() const { return _quality; }
	
private slots:
	
	void onComboBoxActivated(int index);
	void onQualityValueChanged(int quality);
	
private:
	Ui::PaintField_ExportDialog *ui;
	
	WidgetGroup *_group;
	
	QHash<QString, QString> _comboBoxHash;
	QString _format;
	int _quality;
};

}

#endif // FSEXPORTDIALOG_H
