#ifndef FSLAYERUIMODULE_H
#define FSLAYERUIMODULE_H

#include <QObject>
#include "../document/documentmodulebase.h"

namespace PaintField
{

class PanelFrame;
class LayerUIModule;

class LayerUIDocumentModule : public DocumentModuleBase
{
	Q_OBJECT
	friend class LayerUIModule;
	
public:
	
	LayerUIDocumentModule(QObject *parent = 0);
	
	void firstInitialize(Document *document);
	
	void setTreePanel(PanelFrame *panel) { _treePanelFrame = panel; }
	
private:
	
	PanelFrame *_treePanelFrame;
};

class LayerUIModule : public DocumentApplicationModuleBase
{
	Q_OBJECT
public:
	explicit LayerUIModule(QObject *parent = 0);
	
	void initialize();
	
	DocumentModuleBase *createDocumentModule();
	
signals:
	
public slots:
	
private:
	
	PanelFrame *_treePanelFrame;
};

}

#endif // FSLAYERUIMODULE_H
