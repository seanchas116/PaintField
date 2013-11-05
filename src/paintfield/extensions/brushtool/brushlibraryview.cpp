#include "brushlibraryview.h"

#include "brushpresetitem.h"
#include "brushpresetdatabase.h"

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/util.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/dialogs/filedialog.h"
#include "paintfield/core/json.h"
#include "paintfield/core/dialogs/messagebox.h"

#include <QDir>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QAction>
#include <QMenu>

namespace PaintField {

namespace {

const QString itemMimeType = "application/x-paintfield-brushpreset";

void deleteItem(QStandardItemModel *model, QStandardItem *item)
{
	Q_UNUSED(model);
	if (item) {
		auto parent = item->parent();
		if (!parent)
			parent = model->invisibleRootItem();
		qDeleteAll(parent->takeRow(item->row()));
	}
}

void addItems(QStandardItemModel *model, QStandardItem *item, const QList<QStandardItem *> &newItems)
{
	QStandardItem *parent;
	int index;
	if (item) {
		parent = item->parent();
		if (!parent)
			parent = model->invisibleRootItem();
		index = item->row();
	} else {
		parent = model->invisibleRootItem();
		index = parent->rowCount();
	}

	for (auto newItem : newItems) {
		parent->insertRow(index, newItem);
		++index;
	}
}

void addDirectory(QStandardItemModel *model, QStandardItem *item)
{
	auto dir = new BrushPresetItem(true);
	dir->setText(QObject::tr("New Folder"));
	addItems(model, item, {dir});
}

void copyItem(QStandardItemModel *model, QStandardItem *item)
{
	Q_UNUSED(model);
	if (item) {
		QByteArray data;
		QDataStream stream(&data, QIODevice::WriteOnly);
		stream << 1;
		item->write(stream);

		auto mime = new QMimeData();
		mime->setData(itemMimeType, data);

		qApp->clipboard()->setMimeData(mime);
	}
}

void cutItem(QStandardItemModel *model, QStandardItem *item)
{
	copyItem(model, item);
	deleteItem(model, item);
}

void pasteItem(QStandardItemModel *model, QStandardItem *item)
{
	auto mime = qApp->clipboard()->mimeData();

	if (mime->hasFormat(itemMimeType)) {

		auto newItem = new BrushPresetItem();
		{
			auto data = mime->data(itemMimeType);
			QDataStream stream(&data, QIODevice::ReadOnly);
			int count;
			stream >> count;
			newItem->read(stream);
		}

		addItems(model, item, {newItem});
	}
}

void exportItem(QStandardItemModel *model, QStandardItem *item)
{
	Q_UNUSED(model);
	if (!item)
		return;
	auto filePath = FileDialog::getSaveFilePath(nullptr, QObject::tr("Export"), QObject::tr("Brush Presets"), "json");
	if (!filePath.isEmpty()) {
		auto data = BrushPresetItem::saveItemTree({item}, nullptr);
		if (!Json::writeIntoFile(filePath, data)) {
			MessageBox::show(QMessageBox::Warning, QObject::tr("Failed to save file."), QString());
		}
	}
}

void importItem(QStandardItemModel *model, QStandardItem *item)
{
	auto filePath = FileDialog::getOpenFilePath(nullptr, QObject::tr("Import"), QObject::tr("Brush Presets"), {"json"});
	if (!filePath.isEmpty()) {
		auto data = Json::readFromFile(filePath);
		QStandardItem *current;
		auto newItems = BrushPresetItem::loadItemTree(data.toList(), current);
		addItems(model, item, newItems);
	}
}

}

BrushLibraryView::BrushLibraryView(BrushPresetDatabase *database, QWidget *parent) :
    QWidget(parent)
{
	auto layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);

	QMenu *menu;
	
	{
		auto treeView = new QTreeView;
		treeView->setHeaderHidden(true);
		treeView->setModel(database->model());
		treeView->setSelectionModel(database->selectionModel());
		treeView->setDragDropMode(QAbstractItemView::DragDrop);
		treeView->setSelectionMode(QAbstractItemView::SingleSelection);
		treeView->setDefaultDropAction(Qt::MoveAction);
		treeView->setDropIndicatorShown(true);

		menu = new QMenu();

		{
			auto makeAction = [&](const QString &text, const QKeySequence &shortcut) {
				auto action = new QAction(text, this);
				action->setShortcut(shortcut);
				addAction(action);
				return action;
			};

			auto makeActionForItem = [&](void (*op)(QStandardItemModel *, QStandardItem *), const QString &text, const QKeySequence &shortcut) {
				auto action = makeAction(text, shortcut);
				connect(action, &QAction::triggered, database, [=](){
					op(database->model(), database->model()->itemFromIndex(database->selectionModel()->currentIndex()));
				});
				return action;
			};

			auto cut = makeActionForItem(&cutItem, tr("Cut"), QKeySequence("Ctrl+X"));
			auto copy = makeActionForItem(&copyItem, tr("Copy"), QKeySequence("Ctrl+C"));
			auto paste = makeActionForItem(&pasteItem, tr("Paste"), QKeySequence("Ctrl+V"));
			auto del = makeActionForItem(&deleteItem, tr("Delete"), QKeySequence("Delete"));
			auto newDir = makeActionForItem(&addDirectory, tr("Add Folder"), QKeySequence());

			auto restoreBuiltin = makeAction(tr("Re-add Built-in Presets"), QKeySequence());
			connect(restoreBuiltin, &QAction::triggered, [=](){
				QDir builtinSettingsDir = appController()->settingsManager()->builtinDataDir();
				if (builtinSettingsDir.cd("Settings")) {
					database->load(builtinSettingsDir.filePath("brush-presets.json"));
				}
			});

			auto importAction = makeActionForItem(&importItem, tr("Import..."), QKeySequence());
			auto exportAction = makeActionForItem(&exportItem, tr("Export..."), QKeySequence());

			menu->addAction(cut);
			menu->addAction(copy);
			menu->addAction(paste);
			menu->addSeparator();
			menu->addAction(del);
			menu->addSeparator();
			menu->addAction(newDir);
			menu->addSeparator();
			menu->addAction(importAction);
			menu->addAction(exportAction);
			menu->addSeparator();
			menu->addAction(restoreBuiltin);
		}
		
		Util::setExpandTreeViewRecursive(treeView, QModelIndex(), true);

		treeView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(treeView, &QWidget::customContextMenuRequested, [=](const QPoint &pos){
			auto index = treeView->indexAt(pos);
			database->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
			menu->exec(treeView->mapToGlobal(pos));
		});
		
		layout->addWidget(treeView);
	}
	
	{
		auto buttonLayout = new QHBoxLayout;
		buttonLayout->setContentsMargins(6,6,6,6);
		{
			auto button = new SimpleButton(":/icons/16x16/menuDown.svg", QSize(16,16));
			button->setMenu(menu);
			buttonLayout->addWidget(button);
		}
		buttonLayout->addStretch(1);
		layout->addLayout(buttonLayout);
	}
	
	setLayout(layout);
}

} // namespace PaintField
