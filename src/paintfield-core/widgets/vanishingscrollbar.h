#ifndef VANISHINGSCROLLBAR_H
#define VANISHINGSCROLLBAR_H

#include <QAbstractSlider>
#include <tuple>

class QPropertyAnimation;
class QPauseAnimation;

namespace PaintField
{

class CallbackAnimation;

class VanishingScrollBar : public QAbstractSlider
{
	Q_OBJECT
	
public:
	
	constexpr static int barWidth() { return 8; }
	constexpr static int barMargin() { return 2; }
	constexpr static int durationWaiting() { return 1000; }
	constexpr static int durationVanishing() { return 200; }
	constexpr static int totalBarWidth() { return barWidth() + 2 * barMargin(); }
	
	VanishingScrollBar(Qt::Orientation orientation, QWidget *parent = 0);
	
	bool isVanished() const { return _isVanished; }
	double vanishingLevel() const { return _vanishingLevel; }
	
	QRect barRect() const { return _barRect; }
	
	QSize sizeHint() const final;
	
signals:
	
	void vanished();
	
public slots:
	
	void wakeUp();
	
protected:
	
	void sliderChange(SliderChange change) override;
	
	void paintEvent(QPaintEvent *) override;
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
	void wheelEvent(QWheelEvent *);
	
private slots:
	
	void setVanishingLevel(double level)
	{
		_vanishingLevel = level;
		update();
	}
	
	void setVanished(bool x);
	void vanish() { setVanished(true); }
	
private:
	
	void onOrientationChanged();
	
	static QSizePolicy sizePolicyForOrientation(Qt::Orientation orientation);
	static int scrollPos(const QPoint &mousePos, Qt::Orientation orientation);
	static std::tuple<double, double> scrollBarBeginEndPos(int value, int min, int max, int pageStep);
	static QRect scrollBarRect(double begin, double end, const QRect &rect, int margin, Qt::Orientation orientation);
	static QPainterPath scrollBarPath(const QRect &rect);
	
	QRect _barRect;
	bool _isDragged = false;
	int _dragStartPos = 0, _dragStartValue = 0;
	double _vanishingLevel = 0;
	bool _isVanished = true;
	
	QPauseAnimation *_pauseAnimation = 0;
	CallbackAnimation *_vanishingAnimation = 0;
};

}

#endif // VANISHINGSCROLLBAR_H
