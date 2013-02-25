#include "document.h"

#include "selection.h"

namespace PaintField {

class SelectCommand : public QUndoCommand
{
public:
	
	SelectCommand(Selection *selection, Selection::Type type, const QVariant &value, bool inverted, QUndoCommand *parent = 0) :
		QUndoCommand(parent),
		_selection(selection),
		_type(type),
		_value(value),
		_inverted(inverted)
	{}
	
	void redo() { swap(); }
	void undo() { swap(); }
	
public:
	
	void swap()
	{
		auto type = _selection->type();
		auto value = _selection->value();
		auto inverted = _selection->isInverted();
		
		_selection->setSelection(_type, _value, _inverted);
		
		_type = type;
		_value = value;
		_inverted = inverted;
	}
	
	Selection *_selection;
	Selection::Type _type;
	QVariant _value;
	bool _inverted;
};

struct Selection::Data
{
	Document *document = 0;
	
	Type type = TypeNoSelection;
	QVariant value;
	bool inverted;
};

Selection::Selection(Document *document) :
	QObject(document),
	d(new Data)
{
	d->document = document;
}

Selection::~Selection()
{
	delete d;
}

void Selection::setSelection(Type type, const QVariant &value, bool inverted)
{
	d->document->undoStack()->push(new SelectCommand(this, type, value, inverted));
}

Selection::Type Selection::type() const { return d->type; }
QVariant Selection::value() const { return d->value; }
bool Selection::isInverted() const { return d->inverted; }

void Selection::setSelectionDirect(Type type, const QVariant &value, bool inverted)
{
	d->type = type;
	d->value = value;
	d->inverted = inverted;
	
	emit selectionChanged(type, value, inverted);
}

} // namespace PaintField
