#ifndef COLORPANELFACTORY_H
#define COLORPANELFACTORY_H

#include "core/sidebarfactory.h"

namespace PaintField
{

class ColorSidebarFactory : public SidebarFactory
{
	Q_OBJECT
public:
	explicit ColorSidebarFactory(QObject *parent = 0);
	
	QWidget *createSidebar(WorkspaceController *workspace, QWidget *parent);
};

}

#endif // COLORPANELFACTORY_H
