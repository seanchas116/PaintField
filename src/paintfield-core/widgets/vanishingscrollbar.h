#ifndef VANISHINGSCROLLBAR_H
#define VANISHINGSCROLLBAR_H

#include <QAbstractSlider>
#include <tuple>

class QPropertyAnimation;
class QPauseAnimation;

namespace PaintField
{

class VanishingScrollBar : public QAbstractSlider
{
	Q_OBJECT
	
	Q_PROPERTY(double vanishingLevel READ vanishingLevel WRITE setVanishingLevel)
	Q_PROPERTY(bool vanished READ isVanished WRITE setVanished)
	
public:
	
	constexpr static int barWidth() { return 8; }
	constexpr static int barMargin() { return 2; }
	constexpr static int durationWaiting() { return 1000; }
	constexpr static int durationVanishing() { return 200; }
	
	VanishingScrollBar(Qt::Orientation orientation, QWidget *parent = 0);
	
	bool isVanished() const { return _isVanished; }
	QRect barRect() const { return _barRect; }
	
	void setVanishingLevel(double level)
	{
		_vanishingLevel = level;
		update();
	}
	
	double vanishingLevel() const { return _vanishingLevel; }
	
	QSize sizeHint() const final;
	
signals:
	
	void vanished();
	
public slots:
	
	void startAnimation();
	void setVanished(bool x);
	void vanish() { setVanished(true); }
	
protected:
	
	void sliderChange(SliderChange change) override;
	
	void paintEvent(QPaintEvent *) override;
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
	void wheelEvent(QWheelEvent *);
	
private:
	
	void onOrientationChanged();
	
	static QSizePolicy sizePolicyForOrientation(Qt::Orientation orientation);
	static int scrollPos(const QPoint &mousePos, Qt::Orientation orientation);
	static std::tuple<double, double> scrollBarBeginEndPos(int value, int min, int max, int pageStep);
	static QRect scrollBarRect(double begin, double end, const QRect &rect, int margin, Qt::Orientation orientation);
	static QPainterPath scrollBarPath(const QRect &rect, Qt::Orientation orientation);
	
	QRect _barRect;
	bool _isDragged = false;
	int _dragStartPos = 0, _dragStartValue = 0;
	double _vanishingLevel = 0;
	bool _isVanished = true;
	
	QPauseAnimation *_pauseAnimation = 0;
	QPropertyAnimation *_vanishingAnimation = 0;
};

}

#endif // VANISHINGSCROLLBAR_H
