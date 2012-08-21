#ifndef FSEXPORTDIALOG_H
#define FSEXPORTDIALOG_H

#include <QDialog>
#include <QHash>

class FSWidgetGroup;

namespace Ui {
class FSExportDialog;
}

class FSExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit FSExportDialog(QWidget *parent = 0);
	~FSExportDialog();
	
	//QString formatText(const QString &formatName) const { return _hash.value(formatName); }
	QString currentText() const;
	QString currentFormat() const { return _format; }
	int currentQuality() const { return _quality; }
	
private slots:
	
	void onComboBoxActivated(int index);
	void onQualityValueChanged(int quality);
	
private:
	Ui::FSExportDialog *ui;
	
	FSWidgetGroup *_group;
	
	QHash<QString, QString> _hash;
	QString _format;
	int _quality;
};

#endif // FSEXPORTDIALOG_H
