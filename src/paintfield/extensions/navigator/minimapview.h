#pragma once

#include <QWidget>
#include <Malachite/Surface>

namespace PaintField {

class Canvas;

class MinimapView : public QWidget
{
	Q_OBJECT
public:
	explicit MinimapView(Canvas *canvas, QWidget *parent = 0);
	~MinimapView();

signals:

public slots:

protected:

	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *);

private slots:

	void onThumbnailUpdated(const QPointSet &keys);
	void onTransformChanged();

private:

	struct Data;
	Data *d;
};

} // namespace PaintField

