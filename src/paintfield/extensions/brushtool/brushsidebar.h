#pragma once

#include <QWidget>

class QLabel;

namespace PaintField {

class BrushPresetManager;
class BrushPreferencesManager;

class BrushSideBar : public QWidget
{
	Q_OBJECT
public:
	explicit BrushSideBar(BrushPresetManager *presetManager, BrushPreferencesManager *prefManager, QWidget *parent = 0);
	
private:
};

} // namespace PaintField

