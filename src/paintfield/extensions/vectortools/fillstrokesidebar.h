#pragma once

#include "paintfield/core/widgets/abstractlayerpropertyeditor.h"

namespace Malachite
{
class Color;
}

namespace PaintField {

class Workspace;

class FillStrokeSideBar : public AbstractLayerPropertyEditor
{
	Q_OBJECT
public:
	explicit FillStrokeSideBar(Workspace *workspace, LayerScene *scene, QWidget *parent = 0);
	~FillStrokeSideBar();
	
signals:
	
	void fillEnabledChanged(bool enabled);
	void strokeEnabledChanged(bool enabled);
	void strokeWidthChanged(double width);
	void fillColorChanged(const Malachite::Color &color);
	void strokeColorChanged(const Malachite::Color &color);
	
public slots:
	
protected:
	
	void updateForCurrentChange(const LayerRef &current);
	void updateForCurrentPropertyChange() override;
	
private slots:
	
	void onStrokePosChanged(int strokePos);
	void onStrokeJoinChanged(int join);
	void onStrokeCapChanged(int cap);
	void onFillEnabledToggled(bool checked);
	void onStrokeEnabledToggled(bool checked);
	void onStrokeWidthSet(double width);
	void onFillColorSet(const Malachite::Color &color);
	void onStrokeColorSet(const Malachite::Color &color);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
