#ifndef PAINTFIELD_FILLSTROKESIDEBAR_H
#define PAINTFIELD_FILLSTROKESIDEBAR_H

#include <QWidget>

namespace Malachite
{
class Color;
}

namespace PaintField {

class LayerScene;
class LayerRef;
class Workspace;

class FillStrokeSideBar : public QWidget
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
	
	void setCurrentLayer(const LayerRef &current);
	void updateEditor();
	
private slots:
	
	void onStrokePosButtonPressed(int id);
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

#endif // PAINTFIELD_FILLSTROKESIDEBAR_H
