#pragma once

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
	constexpr static int durationWaiting() { return 2000; }
	constexpr static int durationVanishing() { return 200; }
	constexpr static int totalBarWidth() { return barWidth() + 2 * barMargin(); }
	
	VanishingScrollBar(Qt::Orientation orientation, QWidget *parent = 0);
	
	QRect barRect() const { return _barRect; }
	
	QSize sizeHint() const final;
	
	bool isAwake() const { return _isAwake; }
	
signals:
	
	void wokeUp();
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
	
	void setBarOpacity(double level);
	void vanish();
	
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
	double _barOpacity = 0;
	bool _isAwake = false;
	
	QPauseAnimation *_pauseAnimation = 0;
	CallbackAnimation *_vanishingAnimation = 0;
};

}

