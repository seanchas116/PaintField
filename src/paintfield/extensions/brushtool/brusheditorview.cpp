#include "brusheditorview.h"

#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushstroker.h"

#include "paintfield/core/observablevariantmap.h"
#include "paintfield/core/util.h"

#include <QComboBox>
#include <QFormLayout>
#include <QStackedWidget>

namespace PaintField {

struct BrushEditorView::Data
{
	BrushEditorView *mSelf = nullptr;

	QStackedWidget *mEditorStack = nullptr;
	QWidget *mEditor = nullptr;
	BrushStrokerFactoryManager *mStrokerFactoryManager = nullptr;
	BrushPresetManager *mPresetManager = nullptr;

	QComboBox *mStrokerComboBox = nullptr;
	QHash<QString, int> mStrokerNameToComboIndex;

	void createWidgets()
	{
		auto mainLayout = new QVBoxLayout();
		{
			auto layout = new QFormLayout();

			{
				auto strokerCombo = new QComboBox();

				mStrokerFactoryManager->factories()++.eachWithIndex([&](int index, BrushStrokerFactory *factory) {
					strokerCombo->addItem(factory->title());
					mStrokerNameToComboIndex[factory->name()] = index;
				});

				connect(strokerCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
					mPresetManager->setStroker(mStrokerNameToComboIndex.key(index));
				});

				layout->addRow(tr("Type"), strokerCombo);
				mStrokerComboBox = strokerCombo;
			}

			mainLayout->addLayout(layout);
		}

		mEditorStack = new QStackedWidget();
		mainLayout->addWidget(mEditorStack);

		mSelf->setLayout(mainLayout);
	}

	void onStrokerChanged(const QString &strokerName)
	{
		PAINTFIELD_DEBUG;
		// set current combo box index
		mStrokerComboBox->setCurrentIndex(mStrokerNameToComboIndex.value(strokerName, 0));

		// set editor widget

		auto factory = mStrokerFactoryManager->factory(strokerName);

		QWidget *editor = nullptr;
		if (factory)
			editor = factory->createEditor(mPresetManager->parameters());

		if (mEditor) {
			mEditorStack->removeWidget(mEditor);
			mEditor->deleteLater();
		}
		if (editor) {
			Util::applyMacSmallSize(editor);
			mEditorStack->addWidget(editor);
		}
		mEditor = editor;
	}
};

BrushEditorView::BrushEditorView(BrushStrokerFactoryManager *strokerFactoryManager, BrushPresetManager *presetManager, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	using namespace std::placeholders;

	d->mSelf = this;
	d->mStrokerFactoryManager = strokerFactoryManager;
	d->mPresetManager = presetManager;
	d->createWidgets();

	connect(presetManager, &BrushPresetManager::strokerChanged, this, std::bind(&Data::onStrokerChanged, d.data(), _1));
	d->onStrokerChanged(presetManager->stroker());
}

BrushEditorView::~BrushEditorView()
{

}

}
