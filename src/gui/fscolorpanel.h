#ifndef FSCOLORPANEL_H
#define FSCOLORPANEL_H

#include <QtGui>

#include "fspanel.h"
#include "mlcolor.h"

class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QButtonGroup;
class FSSimpleButton;
class FSColorButton;
class FSColorWheel;
class FSColorSlider;
class FSLooseSpinBox;
class QLineEdit;
class QComboBox;
class QGridLayout;
class QLabel;
class FSWidgetGroup;

class FSColorSliderPanel : public QWidget
{
	Q_OBJECT
public:
	explicit FSColorSliderPanel(QWidget *parent = 0);
	
	MLColor color() const { return _color; }
	
public slots:
	void setColor(const MLColor &color);
	
signals:
	void colorChanged(const MLColor &color);
	
private slots:
	void onComboBoxActivated(int index);
	
private:
	
	FSWidgetGroup *_groupRgb, *_groupRgb8, *_groupHsv;
	MLColor _color;
};

class FSWebColorPanel : public QWidget
{
	Q_OBJECT
public:
	explicit FSWebColorPanel(QWidget *parent = 0);
	
	MLColor color() const { return _color; }
	
public slots:
	void setColor(const MLColor &color);
	
signals:
	void colorChanged(const MLColor &color);
	
private slots:
	void onLineEditEditingFinished();
	
private:
	QFormLayout *_layout;
	QLineEdit *_lineEdit;
	
	MLColor _color;
};

class FSColorPanel : public FSPanelWidget
{
	Q_OBJECT
public:
	explicit FSColorPanel(QWidget *parent = 0);
	
	QSize sizeHint() const;
	
signals:
	
public slots:
	void setColor(int index, const MLColor &color);
	void setCurrentIndex(int index);
	
private slots:
	void onColorButtonPressed();
	
private:
	QVBoxLayout *_mainLayout;
	
	QHBoxLayout *_buttonLayout;
	FSSimpleButton *_wheelButton, *_sliderButton, *_webButton, *_dialogButton;
	QButtonGroup *_buttonGroup, *_colorButtonGroup;
	
	QHBoxLayout *_colorButtonLayout;
	QList<FSColorButton *> _colorButtons;
	
	QHBoxLayout *_colorLayout;
	FSColorWheel *_colorWheel;
	FSColorSliderPanel *_sliderPanel;
	FSWebColorPanel *_webColorPanel;
};

#endif // FSCOLORPANEL_H
