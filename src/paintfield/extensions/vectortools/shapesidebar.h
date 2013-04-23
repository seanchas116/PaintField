#ifndef PAINTFIELD_SHAPESIDEBAR_H
#define PAINTFIELD_SHAPESIDEBAR_H

#include <QStackedWidget>

namespace PaintField {

class LayerScene;
class LayerRef;

class ShapeSideBar : public QStackedWidget
{
	Q_OBJECT
public:
	explicit ShapeSideBar(LayerScene *scene, QWidget *parent = 0);
	
signals:
	
	void xChanged(int x);
	void yChanged(int y);
	void widthChanged(int w);
	void heightChanged(int h);
	void rectShapeTypeChanged(int type);
	void textChanged(const QString &text);
	
public slots:
	
private slots:
	
	void onCurrentChanged(const LayerRef &layer);
	
	void onXChanged(int x);
	void onYChanged(int y);
	void onWidthChanged(int w);
	void onHeightChanged(int h);
	
	void onRectShapeTypeChanged(int type);
	void onTextChanged();
	void onFontChangeRequested();
	
	void updateEditors();
	
private:
	
	enum StackIndex
	{
		StackIndexEmpty,
		StackIndexRect,
		StackIndexText
	};
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_SHAPESIDEBAR_H
