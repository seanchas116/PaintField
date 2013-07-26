#pragma once

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
	bool isSmoothEnabled() const { return _isSmoothEnabled; }
	
signals:
	
	void brushSizeChanged(int size);
	void pressureSmoothingEnabledChanged(bool enabled);
	void smoothEnabledChanged(bool enabled);
	
public slots:
	
	void setPresetMetadata(const BrushPresetMetadata &metadata);
	void setBrushSize(int size);
	void setSmoothEnabled(bool enabled);
	
private:
	
	BrushPresetMetadata _metadata;
	int _brushSize = 10;
	bool _isPressureSmoothingEnabled = true;
	bool _isSmoothEnabled = false;
	
	QLabel *_presetLabel = 0;
};

} // namespace PaintField

