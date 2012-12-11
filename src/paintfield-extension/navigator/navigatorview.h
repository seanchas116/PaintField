#ifndef NAVIGATORVIEW_H
#define NAVIGATORVIEW_H

#include <QWidget>

namespace PaintField {

class NavigatorView : public QWidget
{
	Q_OBJECT
	
public:
	explicit NavigatorView(QWidget *parent = 0);
	
	/**
	 * @return Scale
	 */
	double scale() const { return _scale; }
	
	/**
	 * @return Rotation degrees
	 */
	double rotation() const { return _rotationD;}
	
	/**
	 * @return Translation
	 */
	QPoint translation() const { return _translation; }
	
	/**
	 * Sets the log2-scale step of scale.
	 * This value is used when zoom-in or zoom-out button is pressed.
	 * @param step
	 */
	void setScaleLogStep(double step) { _scaleLogStep = step; }
	
	/**
	 * Sets the rotation step.
	 * This value is used when rotate-left or rotate-right button is pressed.
	 * @param step
	 */
	void setRotationStep(int step) { _rotationDStep = step; }
	
signals:
	
	void scaleChanged(double scale);
	void rotationChanged(double rotation);
	void translationChanged(const QPoint &translation);
	void translationXChanged(int x);
	void translationYChanged(int y);
	
	void scaleSliderValueChanged(int value);
	void scalePercentageChanged(double value);
	void rotationSliderValueChanged(int value);
	
public slots:
	
	void setScale(double scale);
	void setRotation(double rotation);
	void setTranslation(const QPoint &translation);
	void setTranslationX(int x) { setTranslation(QPoint(x, _translation.y())); }
	void setTranslationY(int y) { setTranslation(QPoint(_translation.x(), y)); }
	
private slots:
	
	void resetScale() { setScale(1); }
	void resetRotation() { setRotation(0); }
	void zoomOut() { setZoomLevel(zoomLevel() - 1); }
	void zoomIn() { setZoomLevel(zoomLevel() + 1); }
	void rotateLeft() { setRotationLevel(rotationLevel() - 1); }
	void rotateRight() { setRotationLevel(rotationLevel() + 1); }
	
private:
	
	double scaleMin() const { return exp2(_scaleLogMin); }
	double scaleMax() const { return exp2(_scaleLogMax); }
	
	int zoomLevel() const { return qRound(log2(_scale) / _scaleLogStep); }
	void setZoomLevel(int x) { setScale(exp2(x * _scaleLogStep)); }
	
	int rotationLevel() const { return qRound(_rotationD / _rotationDStep); }
	void setRotationLevel(int x) { setRotation(x * _rotationDStep); }
	
	QLayout *createScaleRotationUILayout();
	QLayout *createMiscUILayout();
	void createWidgets(); 
	
	double _scale = 1.0;
	
	QPoint _translation;
	
	double _scaleLogMin = -7, _scaleLogMax = 5;
	double _scaleLogStep = 0.5;
	
	//int _rotationMin = -180 * 16, _rotationMax= 180 * 16;
	//int _rotationStep = 15 * 16;
	
	double _rotationD = 0, _rotationDStep = 15.0;
	
	constexpr static int _spinBoxWidth = 50;
	constexpr static int _scaleSliderResolution = 32;
	
	QPoint _originalTranslation;
};

}

#endif // NAVIGATORVIEW_H
