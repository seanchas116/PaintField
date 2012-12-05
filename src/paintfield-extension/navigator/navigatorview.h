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
	 * @return Rotation (degrees * 16)
	 */
	int rotation() const { return _rotation; }
	
	/**
	 * @return Translation
	 */
	QPoint translation() const { return _translation; }
	
	void setScaleLogStep(double step) { _scaleLogStep = step; }
	void setRotationStep(int step) { _rotationStep = step; }
	
signals:
	
	void scaleChanged(double value);
	void rotationChanged(int value);
	void translationChanged(const QPoint &value);
	void xChanged(int x);
	void yChanged(int y);
	
public slots:
	
	void setScale(double value);
	void setRotation(int value);
	void setTranslation(const QPoint &value);
	void setX(int x) { setTranslation(QPoint(x, _translation.y())); }
	void setY(int y) { setTranslation(QPoint(_translation.x(), y)); }
	
	void revertScale() { setScale(1); }
	void revertRotation() { setRotation(0); }
	void zoomOut() { setZoomLevel(zoomLevel() - 1); }
	void zoomIn() { setZoomLevel(zoomLevel() + 1); }
	void rotateLeft() { setRotationLevel(rotationLevel() - 1); }
	void rotateRight() { setRotationLevel(rotationLevel() + 1); }
	
private slots:
	
private:
	
	double scaleMin() const { return exp2(_scaleLogMin); }
	double scaleMax() const { return exp2(_scaleLogMax); }
	
	int zoomLevel() const { return round(log2(_scale) / _scaleLogStep); }
	void setZoomLevel(int x) { setScale(exp2(x * _scaleLogStep)); }
	
	int rotationLevel() const { return (_rotation + _rotationStep / 2) / _rotationStep; }
	void setRotationLevel(int x) { setRotation(x * _rotationStep); }
	
	QLayout *createScaleRotationUILayout();
	QLayout *createMiscUILayout();
	void createWidgets(); 
	
	double _scale = 1.0;
	int _rotation = 0;
	QPoint _translation;
	
	double _scaleLogMin = -7, _scaleLogMax = 5;
	double _scaleLogStep = 0.5;
	
	int _rotationMin = -180 * 16, _rotationMax= 180 * 16;
	int _rotationStep = 16;
};

}

#endif // NAVIGATORVIEW_H
