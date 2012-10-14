#ifndef FSCOLORPANEL_H
#define FSCOLORPANEL_H

#include <QtGui>

#include "panel.h"
#include "mlcolor.h"

class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QButtonGroup;
class QLineEdit;
class QComboBox;
class QGridLayout;
class QLabel;

namespace PaintField
{

class SimpleButton;
class ColorButton;
class ColorWheel;
class ColorSlider;
class LooseSpinBox;
class WidgetGroup;

class ColorSliderPanel : public QWidget
{
	Q_OBJECT
public:
	explicit ColorSliderPanel(QWidget *parent = 0);
	
	Malachite::Color color() const { return _color; }
	
public slots:
	void setColor(const Malachite::Color &color);
	
signals:
	void colorChanged(const Malachite::Color &color);
	
private slots:
	void onComboBoxActivated(int index);
	
private:
	
	WidgetGroup *_groupRgb, *_groupRgb8, *_groupHsv;
	Malachite::Color _color;
};

class WebColorPanel : public QWidget
{
	Q_OBJECT
public:
	explicit WebColorPanel(QWidget *parent = 0);
	
	Malachite::Color color() const { return _color; }
	
public slots:
	void setColor(const Malachite::Color &color);
	
signals:
	void colorChanged(const Malachite::Color &color);
	
private slots:
	void onLineEditEditingFinished();
	
private:
	QFormLayout *_layout;
	QLineEdit *_lineEdit;
	
	Malachite::Color _color;
};

class ColorPanel : public QWidget
{
	Q_OBJECT
public:
	explicit ColorPanel(QWidget *parent = 0);
	
signals:
	
	void colorChanged(int index, const Malachite::Color &color);
	void currentColorChanged(const Malachite::Color &color);
	
public slots:
	
	void setColor(int index, const Malachite::Color &color);
	void setCurrentColor(const Malachite::Color &color) { setColor(_currentIndex, color); }
	void setCurrentIndex(int index);
	
private slots:
	void onColorButtonPressed();
	
private:
	QVBoxLayout *_mainLayout;
	
	QHBoxLayout *_buttonLayout;
	SimpleButton *_wheelButton, *_sliderButton, *_webButton, *_dialogButton;
	QButtonGroup *_buttonGroup, *_colorButtonGroup;
	
	QHBoxLayout *_colorButtonLayout;
	QList<ColorButton *> _colorButtons;
	
	QHBoxLayout *_colorLayout;
	ColorWheel *_colorWheel;
	ColorSliderPanel *_sliderPanel;
	WebColorPanel *_webColorPanel;
	
	int _currentIndex = 0;
};

}

#endif // FSCOLORPANEL_H
