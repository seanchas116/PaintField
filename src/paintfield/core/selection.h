#pragma once

#include "selectionsurface.h"
#include <QObject>

namespace PaintField {

class Document;

class Selection : public QObject
{
	Q_OBJECT
	
public:
	
	Selection(Document *document);
	~Selection();

	SelectionSurface surface() const;
	
public slots:

	void updateSurface(const SelectionSurface &surface, const QPointSet &keys);
	void commitSurface();
signals:

	void surfaceChanged(const SelectionSurface &surface, const QPointSet &keys);
	
private:
	
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

