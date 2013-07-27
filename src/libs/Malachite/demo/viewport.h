#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>
#include <Malachite/Surface>
#include <Malachite/Image>

class Viewport : public QWidget
{
	Q_OBJECT
public:
	
	enum Mode
	{
		ModeImage,
		ModeSurface
	};
	
	explicit Viewport(QWidget *parent = 0);
	
	void clearPaintable();
	
	void setMode(Mode mode) { _mode = mode; update(); }
	Malachite::Image *image() { return &_image; }
	Malachite::Surface *surface() { return &_surface; }
	Malachite::Paintable *paintable()
	{
		if (_mode == ModeImage)
			return &_image;
		else
			return &_surface;
	}
	
signals:
	
public slots:
	
protected:
	
	void paintEvent(QPaintEvent *);
	
private:
	
	Mode _mode = ModeSurface;
	Malachite::Image _image;
	Malachite::Surface _surface;
};

#endif // VIEWPORT_H
