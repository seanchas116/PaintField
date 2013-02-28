#ifndef PAINTFIELD_GENERALEDITACTION_H
#define PAINTFIELD_GENERALEDITACTION_H

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

#endif // PAINTFIELD_GENERALEDITACTION_H
