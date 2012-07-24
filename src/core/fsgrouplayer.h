#ifndef FSGROUPLAYER_H
#define FSGROUPLAYER_H

#include "fslayer.h"

class FSGroupLayer : public FSLayer
{
public:
	FSGroupLayer(const QString &name = QString()) : FSLayer(name) {}
	FSGroupLayer(const FSGroupLayer &other) : FSLayer(other) {}
	
	FSLayer *clone() const { return new FSGroupLayer(*this); }
	Type type() const { return FSLayer::TypeGroup; }
	bool canHaveChildren() const { return true; }
	
	void updateThumbnail(const QSize &size);
};

#endif // FSGROUPLAYER_H
