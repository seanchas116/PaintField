#include "redoundoaction.h"

namespace PaintField {

struct RedoUndoAction::Data
{
	QString prefix, description;
};

RedoUndoAction::RedoUndoAction(QObject *parent) :
	QAction(parent),
	d(new Data)
{
}

RedoUndoAction::~RedoUndoAction()
{
	delete d;
}

void RedoUndoAction::setPrefix(const QString &text)
{
	d->prefix = text;
	setText(text + d->description);
}

QString RedoUndoAction::prefix() const
{
	return d->prefix;
}

QString RedoUndoAction::description() const
{
	return d->description;
}

void RedoUndoAction::setDescription(const QString &text)
{
	d->description = text;
	setText(d->prefix + text);
}

} // namespace PaintField
