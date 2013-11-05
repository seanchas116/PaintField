#pragma once

#include <QWidget>

class QItemSelectionModel;
class QModelIndex;

namespace PaintField {

class BrushPresetDatabase;

class BrushLibraryView : public QWidget
{
	Q_OBJECT
public:
	BrushLibraryView(BrushPresetDatabase *database, QWidget *parent = 0);
	
signals:
	
	void saveRequested();
	void reloadRequested();
	
public slots:
	
private:
	
	
};

} // namespace PaintField

