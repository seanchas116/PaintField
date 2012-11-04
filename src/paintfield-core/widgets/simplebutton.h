#ifndef FSSIMPLEBUTTON_H
#define FSSIMPLEBUTTON_H

#include <QToolButton>
#include <QMargins>
#include <QPixmap>

namespace PaintField
{



class SimpleButton : public QToolButton
{
	Q_OBJECT
public:
	explicit SimpleButton(QWidget *parent = 0);
	SimpleButton(const QIcon &icon, QWidget *parent = 0);
	SimpleButton(const QString &basePixmapFile, const QSize &size, QWidget *parent = 0);
	
	QSize sizeHint() const;
	
	void setMargins(const QMargins &margins) { _margins = margins; }
	void setMargins(int left, int top, int right, int bottom) { setMargins(QMargins(left, top, right, bottom)); }
	QMargins margin() const { return _margins; }
	
	static QIcon createSimpleIconSet(const QString &basePixmapFile, const QSize &size);
	static QIcon createSimpleIconSet(const QPixmap &basePixmap, const QSize &size);
	
signals:
	
public slots:
	
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	
	void paintEvent(QPaintEvent *);
	
private:
	
	QMargins _margins;
	bool _active = false;
};

}

#endif // FSSIMPLEBUTTON_H
