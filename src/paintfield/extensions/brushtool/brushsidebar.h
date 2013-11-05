#pragma once

#include <QWidget>

class QLabel;

namespace PaintField {

class BrushPresetManager;

class BrushSideBar : public QWidget
{
	Q_OBJECT
public:
	explicit BrushSideBar(BrushPresetManager *presetManager, QWidget *parent = 0);
	
private:
};

} // namespace PaintField

