#include <QApplication>
#include <QCursor>
#include <QPair>
#include <Malachite/List>

#include "cursorstack.h"

using namespace Malachite;

namespace PaintField {

typedef QPair<QString, QCursor> IdAndCursor;

struct CursorStack::Data
{
	bool enabled;
	QCursor cursor;
	
	List<IdAndCursor> cursorStack;
};

CursorStack::CursorStack(QObject *parent) :
	QObject(parent),
	d(new Data)
{
}

CursorStack::~CursorStack()
{
	delete d;
}

void CursorStack::setEnabled(bool enabled)
{
	if (d->enabled != enabled)
	{
		d->enabled = enabled;
		
		if (enabled)
			qApp->setOverrideCursor(d->cursor);
		else
			qApp->restoreOverrideCursor();
	}
}

bool CursorStack::isEnabled() const
{
	return d->enabled;
}

void CursorStack::add(const QString &id, const QCursor &cursor)
{
	d->cursorStack.removeIf([id](const IdAndCursor &x){return x.first == id;});
	d->cursorStack << IdAndCursor(id, cursor);
	updateCursor();
}

void CursorStack::remove(const QString &id)
{
	d->cursorStack.removeIf([id](const IdAndCursor &x){return x.first == id;});
	updateCursor();
}

void CursorStack::setCursor(const QCursor &cursor)
{
	d->cursor = cursor;
	
	if (d->enabled)
		qApp->changeOverrideCursor(cursor);
}

void CursorStack::updateCursor()
{
	if (d->cursorStack.size())
	{
		setEnabled(true);
		setCursor(d->cursorStack.last().second);
	}
	else
	{
		setEnabled(false);
		setCursor(QCursor());
	}
}

} // namespace PaintField
