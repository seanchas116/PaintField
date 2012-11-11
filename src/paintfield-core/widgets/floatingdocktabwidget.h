#ifndef FLOATINGDOCKTABWIDGET_H
#define FLOATINGDOCKTABWIDGET_H

#include "docktabwidget.h"

namespace PaintField
{

class FloatingDockTabWidget : public DockTabWidget
{
	Q_OBJECT
public:
	
	/**
	 * Constructs a FloatingDockTabWidget.
	 * baseWindow must have slots focusChanged(bool) to automatically show/hide this widget.
	 * @param baseWindow
	 * @param parent
	 */
	FloatingDockTabWidget(QWidget *baseWindow, QWidget *parent = 0);
	FloatingDockTabWidget(FloatingDockTabWidget *other, QWidget *parent);
	
	QWidget *baseWindow() { return _baseWindow; }
	
signals:
	
protected:
	
private slots:
	
	void onFocusChanged(QWidget *old, QWidget *now);
	
private:
	
	void commonInit();
	
	QWidget *_baseWindow;
	bool _hiddenByFocusChange = false;
};

}

#endif // FLOATINGDOCKTABWIDGET_H
