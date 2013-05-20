#include <QItemSelectionModel>
#include <QFileDialog>

#include "paintfield/core/json.h"
#include "paintfield/core/util.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/application.h"
#include "paintfield/core/settingsmanager.h"
#include "brushlibrarymodel.h"
#include "brushpresetmanager.h"
#include "brushlibraryview.h"

#include "brushlibrarycontroller.h"

namespace PaintField {

BrushLibraryController::BrushLibraryController(BrushPresetManager *presetManager, QObject *parent) :
    QObject(parent),
    _presetManager(presetManager)
{
	_model = new BrushLibraryModel(this);
	
	_model->addRootPath(QDir(appController()->settingsManager()->builtinDataDir()).filePath("Contents/Brush Presets"), "Built-in");
	_model->addRootPath(QDir(appController()->settingsManager()->userDataDir()).filePath("Contents/Brush Presets"), "User");
	
	_selectionModel = new QItemSelectionModel(_model, this);
	
	auto view = new BrushLibraryView(_model, _selectionModel);
	
	connect(appController()->app(), SIGNAL(tabletPointerChanged(TabletPointerInfo,TabletPointerInfo)), this, SLOT(onCurrentTabletPointerChanged(TabletPointerInfo,TabletPointerInfo)));
	
	connect(_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));
	onCurrentChanged(_selectionModel->currentIndex(), QModelIndex());
	
	//connect(view, SIGNAL(saveRequested()), this, SLOT(onSaveRequested()));
	connect(view, SIGNAL(reloadRequested()), this, SLOT(onReloadRequested()));
	
	_view.reset(view);
	
	{
		_defaultPenItem = _model->itemFromIndex(_model->findIndex({"Built-in", "Pen", "Pen"}));
		_defaultEraserItem = _model->itemFromIndex(_model->findIndex({"Built-in", "Pen", "Eraser"}));
		setCurrentItem(_defaultPenItem);
	}
}

void BrushLibraryController::onCurrentTabletPointerChanged(const TabletPointerInfo &curr, const TabletPointerInfo &prev)
{
	_itemHash[prev] = _model->itemFromIndex(_selectionModel->currentIndex());
	
	if (_itemHash.contains(curr))
	{
		setCurrentItem(_itemHash[curr]);
	}
	else
	{
		if (curr.type == QTabletEvent::Eraser)
			setCurrentItem(_defaultEraserItem);
		else
			setCurrentItem(_defaultPenItem);
	}
}

void BrushLibraryController::onCurrentChanged(const QModelIndex &index, const QModelIndex &prev)
{
	auto data = _model->loadPreset(index);
	if (!data.isEmpty())
		_presetManager->setPreset(data);
	
	emit currentItemChanged(_model->itemFromIndex(index), _model->itemFromIndex(prev));
}

void BrushLibraryController::onSaveRequested()
{
	auto currentIndex = _selectionModel->currentIndex();
	auto item = _model->itemFromIndex(currentIndex);
	
	QString dirPath = _model->dirPathFromItem(item);
	if (dirPath.isNull())
	{
		PAINTFIELD_DEBUG << "invalid directory path";
		return;
	}
	
	QDir dir(dirPath);
	
	if (!dir.exists())
	{
		PAINTFIELD_DEBUG << "invalid directory path";
		return;
	}
	
	auto filePath = QFileDialog::getSaveFileName(0,
	                                             tr("Save Preset"),
	                                             dir.filePath(_presetManager->metadata().title()),
	                                             tr("Preset File (*.json)"));
	
	Json::writeIntoFile(filePath, _presetManager->preset());
}

void BrushLibraryController::onReloadRequested()
{
	auto item = _model->itemFromIndex(_selectionModel->currentIndex());
	_model->updateDirItem(_model->itemDir(item));
}

void BrushLibraryController::setCurrentItem(QStandardItem *item)
{
	_selectionModel->setCurrentIndex(_model->indexFromItem(item), QItemSelectionModel::SelectCurrent);
}

} // namespace PaintField
