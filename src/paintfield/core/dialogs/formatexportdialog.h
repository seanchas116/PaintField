#pragma once

#include <QDialog>

namespace PaintField {

class FormatSupport;

class FormatExportDialog : public QDialog
{
	Q_OBJECT
public:
	explicit FormatExportDialog(const QList<FormatSupport *> &formatSupports, QWidget *parent = 0);
	~FormatExportDialog();
	
	FormatSupport *currentFormatSupport();
	QWidget *currentOptionWidget();
	
signals:
	
public slots:
	
private slots:
	
	void onCurrentIndexChanged(int index);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
