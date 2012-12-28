#ifndef PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H
#define PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include "paintfield-core/smartpointer.h"

#include "brushlibraryview.h"

class QColumnView;
class QItemSelectionModel;

namespace PaintField {

class BrushLibraryModel;
class BrushLibraryView;
class BrushPresetManager;

class BrushLibraryController : public QObject
{
	Q_OBJECT
public:
	explicit BrushLibraryController(BrushPresetManager *presetManager, QObject *parent = 0);
	
	QWidget *view() { return _view.data(); }
	
signals:
	
public slots:
	
private slots:
	
	void onCurrentChanged(const QModelIndex &index);
	void onSaveRequested();
	void onReloadRequested();
	
private:
	
	BrushPresetManager *_presetManager = 0;
	BrushLibraryModel *_model = 0;
	QItemSelectionModel *_selectionModel = 0;
	
	ScopedQObjectPointer<BrushLibraryView> _view;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H
