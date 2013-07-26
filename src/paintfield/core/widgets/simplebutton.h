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
	
	QSize sizeHint() const;
	
	void setMargins(const QMargins &margins) { _margins = margins; }
	void setMargins(int left, int top, int right, int bottom) { setMargins(QMargins(left, top, right, bottom)); }
	QMargins margin() const { return _margins; }
	
	static QIcon createIcon(const QPixmap &basePixmap, const QBrush &onBrush = QBrush(), const QBrush &offBrush = QBrush());
	static QIcon createIcon(const QString &path, const QBrush &onBrush = QBrush(), const QBrush &offBrush = QBrush());
	
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

