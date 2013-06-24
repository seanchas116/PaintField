#pragma once

#include <QDialog>
#include <QHash>
#include "global.h"

class QListWidgetItem;
class QDialogButtonBox;
class QListWidget;

namespace PaintField {

class WidgetGroup;

class JpegForm : public QWidget
{
	Q_OBJECT
	
public:
	
	JpegForm(QWidget *parent = 0);
	
	int quality() const { return _quality; }
	
private slots:
	
	void onQualityChanged(int quality) { _quality = quality; }
	
private:
	
	int _quality = 90;
};

class PngForm : public QWidget
{
	Q_OBJECT
	
public:
	
	PngForm(QWidget *parent = 0);
	
	bool isAlphaEnabled() const { return _alphaEnabled; }
	
private slots:
	
	void onAlphaChanged(bool enabled) { _alphaEnabled = enabled; }
	
private:
	
	int _alphaEnabled = true;
};

class ExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ExportDialog(QWidget *parent = 0);
	
	QString currentText() const;
	QString currentFormat() const { return _format; }
	int currentQuality() const { return _jpegForm->quality(); }
	bool isAlphaEnabled() const { return _pngForm->isAlphaEnabled(); }
	
private slots:
	
	void onFormatItemChanged(QListWidgetItem *item);
	
private:
	
	QHash<QListWidgetItem *, QString> _formatsForItems;
	
	QDialogButtonBox *_buttonBox = 0;
	QListWidget *_listWidget = 0;
	JpegForm *_jpegForm = 0;
	PngForm *_pngForm = 0;
	QString _format;
};

}
