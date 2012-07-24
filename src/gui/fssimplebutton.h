#ifndef FSSIMPLEBUTTON_H
#define FSSIMPLEBUTTON_H

#include <QToolButton>
#include <QMargins>

QIcon fsCreateSimpleIconSet(const QString &basePixmapFile);
QIcon fsCreateSimpleIconSet(const QPixmap &basePixmap);

class FSSimpleButton : public QToolButton
{
	Q_OBJECT
public:
	explicit FSSimpleButton(QWidget *parent = 0);
	FSSimpleButton(const QIcon &icon, QWidget *parent = 0);
	FSSimpleButton(const QString &basePixmapFile, QWidget *parent = 0);
	
	QSize sizeHint() const;
	
	void setMargins(const QMargins &margins) { _margins = margins; }
	void setMargins(int left, int top, int right, int bottom) { setMargins(QMargins(left, top, right, bottom)); }
	QMargins margin() const { return _margins; }
	
signals:
	
public slots:
	
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	
	void paintEvent(QPaintEvent *);
	
private:
	
	void commonInit();
	
	QMargins _margins;
	bool _active;
};

#endif // FSSIMPLEBUTTON_H
