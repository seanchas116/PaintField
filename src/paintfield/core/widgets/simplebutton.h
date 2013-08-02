#pragma once

#include <QToolButton>
#include <QMargins>
#include <QPixmap>

#include "../global.h"

namespace PaintField
{



class SimpleButton : public QToolButton
{
	Q_OBJECT
	
public:
	explicit SimpleButton(QWidget *parent = 0);
	explicit SimpleButton(const QIcon &icon, QWidget *parent = 0);
	SimpleButton(const QString &basePixmapFile, const QSize &size, QWidget *parent = 0);
	SimpleButton(const QString &basePixmapFile, const QSize &size, QObject *obj, const char *slot, QWidget *parent = 0);
	
	~SimpleButton();
	
	QSize sizeHint() const;
	
	void setPressable(bool pressable);
	bool isPressable() const;
	
	void setMargins(const QMargins &margins);
	void setMargins(int left, int top, int right, int bottom) { setMargins(QMargins(left, top, right, bottom)); }
	QMargins margins() const;
	
	static QIcon createIcon(const QPixmap &basePixmap, const QBrush &onBrush = QBrush(), const QBrush &offBrush = QBrush());
	static QIcon createIcon(const QString &path, const QBrush &onBrush = QBrush(), const QBrush &offBrush = QBrush());
	
signals:
	
public slots:
	
protected:
	
	bool hitButton(const QPoint &pos) const;
	
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	
	void paintEvent(QPaintEvent *);
	
private:
	
	struct Data;
	Data *d;
};

}

