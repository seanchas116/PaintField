#ifndef PAINTFIELD_BRUSHSIDEBAR_H
#define PAINTFIELD_BRUSHSIDEBAR_H

#include <QWidget>
#include "brushpresetmanager.h"

class QLabel;

namespace PaintField {

class BrushSideBar : public QWidget
{
	Q_OBJECT
public:
	explicit BrushSideBar(QWidget *parent = 0);
	
	int brushSize() const { return _brushSize; }
	bool isPressureSmoothingEnabled() const { return _isPressureSmoothingEnabled; }
	
signals:
	
	void brushSizeChanged(int size);
	void pressureSmoothingEnabledChanged(bool enabled);
	
public slots:
	
	void setPresetMetadata(const BrushPresetMetadata &metadata);
	void setBrushSize(int size);
	
private:
	
	BrushPresetMetadata _metadata;
	int _brushSize = 10;
	bool _isPressureSmoothingEnabled = true;
	
	QLabel *_presetLabel = 0;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHSIDEBAR_H
