#ifndef FSPALETTEMODULE_H
#define FSPALETTEMODULE_H

#include <QObject>
#include "core/application.h"
#include "core/applicationmodulebase.h"
#include "palettemanager.h"

namespace PaintField
{

class PaletteModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit PaletteModule(QObject *parent = 0);
	
	void initialize();
	
	static PaletteManager *paletteManager() { return app()->findChild<PaletteManager *>(); }
	
signals:
	
public slots:
	
private:
};

}


#endif // FSPALETTEMODULE_H
