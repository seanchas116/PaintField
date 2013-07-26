#pragma once

#include <QAbstractButton>
#include <Malachite/Color>

namespace PaintField
{

class ColorButton : public QAbstractButton
{
	Q_OBJECT
public:
	
	enum
	{
		ButtonSize = 20,
		ButtonMargin = 2
	};
	
	explicit ColorButton(QWidget *parent = 0);
	~ColorButton();
	
	QSize sizeHint() const;
	Malachite::Color color() const;
	
signals:
	
	void colorChanged(const Malachite::Color &c);
	void colorEditingFinished(const Malachite::Color &c);
	
public slots:
	
	void setColor(const Malachite::Color &c);
	void finishEditingColor();
	void copyColor();
	void pasteColor();
	void undo();
	void redo();
	
protected:
	
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void contextMenuEvent(QContextMenuEvent *);
	void paintEvent(QPaintEvent *e);
	
private:
	
	struct Data;
	Data *d;
};

}

