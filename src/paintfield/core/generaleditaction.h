#pragma once

#include "proxyaction.h"

namespace PaintField {

class GeneralEditAction : public ProxyAction
{
	Q_OBJECT
public:
	
	explicit GeneralEditAction(QObject *parent);
	GeneralEditAction(const QString &id, QObject *parent);
	
private slots:
	
	void onFocusChanged(QWidget *old, QWidget *now);
	
};

} // namespace PaintField

