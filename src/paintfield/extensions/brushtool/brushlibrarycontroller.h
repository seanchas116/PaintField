#ifndef PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H
#define PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include "paintfield/core/tabletpointerinfo.h"
#include "paintfield/core/smartpointer.h"

#include "brushlibraryview.h"

class QColumnView;
class QItemSelectionModel;
class QStandardItem;

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
	
	void currentItemChanged(QStandardItem *item, QStandardItem *prev);
	
public slots:
	
private slots:
	
	void onCurrentTabletPointerChanged(const TabletPointerInfo &curr, const TabletPointerInfo &prev);
	void onCurrentChanged(const QModelIndex &index, const QModelIndex &prev);
	void onSaveRequested();
	void onReloadRequested();
	
private:
	
	void setCurrentItem(QStandardItem *item);
	
	BrushPresetManager *_presetManager = 0;
	BrushLibraryModel *_model = 0;
	QItemSelectionModel *_selectionModel = 0;
	
	ScopedQObjectPointer<BrushLibraryView> _view;
	
	QHash<TabletPointerInfo, QStandardItem *> _itemHash;
	QStandardItem *_defaultPenItem = 0, *_defaultEraserItem = 0;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSH_BRUSHLIBRARYCONTROLLER_H
