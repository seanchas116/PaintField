#pragma once
#include "paintfield/core/mvvmviewmodel.h"

namespace PaintField {

class ObservableVariantMap;

class SelectionBrushSidebarViewModel : public MVVMViewModel
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
public:
	explicit SelectionBrushSidebarViewModel(ObservableVariantMap *state, QObject *parent);
	
	void setTitle(const QString &title);
	QString title() const { return mTitle; }
	
signals:
	void titleChanged(const QString &);
	
private:
	QString mTitle;
};

} // namespace PaintField
