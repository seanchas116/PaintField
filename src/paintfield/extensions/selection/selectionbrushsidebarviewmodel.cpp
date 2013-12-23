#include "selectionbrushsidebarviewmodel.h"
#include "paintfield/core/observablevariantmap.h"
#include "paintfield/core/toolmanager.h"

namespace PaintField {

SelectionBrushSidebarViewModel::SelectionBrushSidebarViewModel(ObservableVariantMap *state, QObject *parent) :
	MVVMViewModel(parent)
{
	route(state->customProperty("size"), "size");
	route(state->customProperty("smooth"), "smooth");
	route(this, "title", "title");
}

void SelectionBrushSidebarViewModel::setTitle(const QString &title)
{
	if (mTitle != title) {
		mTitle = title;
		emit titleChanged(title);
	}
}

} // namespace PaintField

