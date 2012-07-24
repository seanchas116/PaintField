#ifndef FSCORE_H
#define FSCORE_H

#include <QObject>

#include "fstoolmanager.h"
#include "fspalettemanager.h"

class FSCore : public QObject
{
	Q_OBJECT
public:
	FSCore(QObject *parent = 0);
	
	FSToolManager *toolManager() { return _toolManager; }
	FSPaletteManager *paletteManager() { return _paletteManager; }
	
	static FSCore *instance() { return _instance; }
	
signals:
	
public slots:
	
private:
	FSToolManager *_toolManager;
	FSPaletteManager *_paletteManager;
	
	static FSCore *_instance;
};

inline FSCore *fsCore() { return FSCore::instance(); }
inline FSToolManager *fsToolManager() { return fsCore()->toolManager(); }
inline FSPaletteManager *fsPaletteManager() { return fsCore()->paletteManager(); }

#endif // FSCORE_H
