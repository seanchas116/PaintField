#ifndef PAINTFIELD_VANISHINGSCROLLAREA_H
#define PAINTFIELD_VANISHINGSCROLLAREA_H

#include <QWidget>

namespace PaintField {

class VanishingScrollBar;

class VanishingScrollArea : public QWidget
{
	Q_OBJECT
public:
	explicit VanishingScrollArea(QWidget *parent = 0);
	~VanishingScrollArea();
	
	VanishingScrollBar *scrollBarX();
	VanishingScrollBar *scrollBarY();
	
signals:
	
public slots:
	
protected:
	
	void resizeEvent(QResizeEvent *event);
	
private:
	
	void moveScrollBars();
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_VANISHINGSCROLLAREA_H
