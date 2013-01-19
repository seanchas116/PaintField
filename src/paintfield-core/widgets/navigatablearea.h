#ifndef PAINTFIELD_NAVIGATABLEAREA_H
#define PAINTFIELD_NAVIGATABLEAREA_H

#include <QWidget>

namespace PaintField {

class VanishingScrollBar;

class NavigatableArea : public QWidget
{
	Q_OBJECT
	
	Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
	Q_PROPERTY(double rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
	Q_PROPERTY(QPoint translation READ translation WRITE setTranslation NOTIFY translationChanged)
	
	Q_PROPERTY(QPoint maxAbsTranslation READ maxAbsTranslation)
	
	Q_PROPERTY(QSize sceneSize READ sceneSize WRITE setSceneSize)
	
	Q_PROPERTY(QTransform transformToScene READ transformToScene)
	Q_PROPERTY(QTransform transformFromScene READ transformFromScene)
	Q_PROPERTY(QTransform navigatorTransform READ navigatorTransform)
	
public:
	
	explicit NavigatableArea(QWidget *parent = 0);
	
	double scale() const { return _scale; }
	double rotation() const { return _rotation; }
	QPoint translation() const { return _translation; }
	
	bool isMirroringEnabled() const { return _mirroringEnabled; }
	
	QPoint maxAbsTranslation() const { return _maxAbsTranslation; }
	
	QSize sceneSize() const { return _sceneSize; }
	void setSceneSize(const QSize &size) { _sceneSize = size; }
	
	QTransform transformToScene() const { return _transformToScene; }
	QTransform transformFromScene() const { return _transformFromScene; }
	
	QTransform navigatorTransform() const { return _navigatorTransform; }
	
	QPoint viewCenter() const { return _viewCenter; }
	
	void memorizeTransform();
	void restoreTransform();
	
public slots:
	
	/**
	 * Sets the scale of the canvas, with a central point on its center.
	 * translation is not changed.
	 * @param value
	 */
	void setScale(double value);
	
	/**
	 * Sets the scale of the canvas, with a central point on the center of the view.
	 * translation is changed proportionally.
	 * @param value
	 */
	void setViewScale(double value);
	
	/**
	 * Sets the rotation of the canvas, with a central point on its center.
	 * @param value
	 */
	void setRotation(double value);
	
	/**
	 * Sets the rotation of the canvas, with a central point on the center of the view.
	 * @param value
	 */
	void setViewRotation(double value);
	
	/**
	 * Sets the translation of the canvas.
	 * @param value
	 */
	void setTranslation(const QPoint &value);
	void setTranslation(int x, int y) { setTranslation(QPoint(x, y)); }
	
	void setMirroringEnabled(bool enabled);
	
signals:
	
	void scaleChanged(double value);
	void rotationChanged(double value);
	void translationChanged(const QPoint &value);
	void maxAbsTranslationChanged(const QPoint &value);
	void mirroringEnabledChanged(bool enabled);
	
protected:
	
	void wheelEvent(QWheelEvent *event);
	void resizeEvent(QResizeEvent *event);
	
private slots:
	
	void onScrollBarXChanged(int value);
	void onScrollBarYChanged(int value);
	
private:
	
	void updateTransforms();
	void updateScrollBarValue();
	void updateScrollBarRange();
	void moveScrollBars();
	
	VanishingScrollBar *_scrollBarX, *_scrollBarY;
	
	QTransform _transformToScene, _transformFromScene, _navigatorTransform;
	
	double _scale = 1.0;
	double _rotation = 0.0;
	QPoint _translation;
	
	bool _mirroringEnabled = false;
	
	double _backupScale = 1.0;
	double _backupRotation = 0.0;
	QPoint _backupTranslation;
	
	QPoint _maxAbsTranslation;
	
	QPoint _viewCenter;
	
	QSize _sceneSize;
	
	double _memorizedScale = 1.0;
	double _memorizedRotation = 1.0;
	QPoint _memorizedTranslation;
};

} // namespace PaintField

#endif // PAINTFIELD_NAVIGATABLEAREA_H
