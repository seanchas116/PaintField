#pragma once

#include "paintfield/core/widgets/abstractlayerpropertyeditor.h"

namespace PaintField {

class LayerScene;

class ShapeSideBar : public AbstractLayerPropertyEditor
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
	
protected:
	
	void updateForCurrentChange(const LayerConstPtr &current) override;
	void updateForCurrentPropertyChange() override;
	
private slots:
	
	void onXChanged(int x);
	void onYChanged(int y);
	void onWidthChanged(int w);
	void onHeightChanged(int h);
	
	void onTextChanged();
	
	void onFontSelectRequested();
	void onFontItalicChanged(bool italic);
	
	void onTextAlignmentChanged(int alignment);
	
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
