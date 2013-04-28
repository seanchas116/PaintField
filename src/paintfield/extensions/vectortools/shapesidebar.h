#pragma once

#include <QWidget>

namespace PaintField {

class LayerScene;
class LayerRef;

class ShapeSideBar : public QWidget
{
	Q_OBJECT
public:
	explicit ShapeSideBar(LayerScene *scene, QWidget *parent = 0);
	
signals:
	
	void xChanged(int x);
	void yChanged(int y);
	void widthChanged(int w);
	void heightChanged(int h);
	
	void textChanged(const QString &text);

	void fontItalicChanged(bool italic);
	
public slots:
	
private slots:
	
	void onCurrentChanged(const LayerRef &layer);
	
	void onXChanged(int x);
	void onYChanged(int y);
	void onWidthChanged(int w);
	void onHeightChanged(int h);
	
	void onTextChanged();
	
	void onFontSelectRequested();
	void onFontItalicChanged(bool italic);
	
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
