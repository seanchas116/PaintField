#include "brusheditorview.h"

#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushstroker.h"

#include <QVBoxLayout>

namespace PaintField {

BrushEditor::BrushEditor(QWidget *parent) :
	QWidget(parent)
{}

struct BrushEditorView::Data
{
	QLayout *m_layout;
	BrushEditor *m_oldEditor = nullptr;
	BrushStrokerFactoryManager *m_strokerFactoryManager = nullptr;
	BrushPresetManager *m_presetManager = nullptr;

	void onStrokerChanged()
	{
		auto stroker = m_presetManager->stroker();
		auto factory = m_strokerFactoryManager->factory(stroker);

		BrushEditor *editor = nullptr;
		if (factory)
			editor = factory->createEditor(m_presetManager->settings());

		if (m_oldEditor)
			(m_oldEditor)->deleteLater();
		if (editor) {
			m_layout->addWidget(editor);
			connect(editor, &BrushEditor::settingsChanged, m_presetManager, &BrushPresetManager::setSettings);
		}
		(m_oldEditor) = editor;
	}

	void onSettingsChanged(const QVariantMap &settings)
	{
		if (m_oldEditor) {
			m_oldEditor->setSettings(settings);
		}
	}
};

BrushEditorView::BrushEditorView(BrushStrokerFactoryManager *strokerFactoryManager, BrushPresetManager *presetManager, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	using namespace std::placeholders;

	d->m_strokerFactoryManager = strokerFactoryManager;
	d->m_presetManager = presetManager;
	d->m_layout = new QVBoxLayout();
	setLayout(d->m_layout);

	connect(presetManager, &BrushPresetManager::strokerChanged, this, std::bind(&Data::onStrokerChanged, d.data()));
	connect(presetManager, &BrushPresetManager::settingsChanged, this, std::bind(&Data::onSettingsChanged, d.data(), _1));
}

BrushEditorView::~BrushEditorView()
{

}

}
