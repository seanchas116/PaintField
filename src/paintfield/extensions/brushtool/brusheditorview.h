#ifndef BRUSHEDITORVIEW_H
#define BRUSHEDITORVIEW_H

#include <QWidget>

namespace PaintField {

class BrushPresetManager;
class BrushStrokerFactoryManager;

class BrushEditorView : public QWidget
{
	Q_OBJECT
public:
	explicit BrushEditorView(BrushStrokerFactoryManager *strokerFactoryManager, BrushPresetManager *presetManager, QWidget *parent = 0);
	~BrushEditorView();

signals:

private:

	struct Data;
	QScopedPointer<Data> d;
};

}

#endif // BRUSHEDITORVIEW_H
