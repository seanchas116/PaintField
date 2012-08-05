#include "fslayertreeview.h"

void FSLayerTreeView::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	emit windowFocused();
}

