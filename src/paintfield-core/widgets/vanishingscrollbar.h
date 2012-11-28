#ifndef VANISHINGSCROLLBAR_H
#define VANISHINGSCROLLBAR_H

#include <QAbstractSlider>
#include <tuple>

class VanishingScrollBar : public QAbstractSlider
{
	Q_OBJECT
public:
	
	constexpr static int barWidth() { return 8; }
	constexpr static int barMargin() { return 2; }
	
	VanishingScrollBar(Qt::Orientation orientation, QWidget *parent = 0);
	
	QRect barRect() const { return _barRect; }
	
	QSize sizeHint() const final;
	
signals:
	
public slots:
	
protected:
	
	void sliderChange(SliderChange change) override;
	
	void paintEvent(QPaintEvent *) override;
	
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
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
};

#endif // VANISHINGSCROLLBAR_H
