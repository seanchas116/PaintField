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
	void commitUpdates();

signals:
	void updated(const QPointSet &keys);
	
private:
	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

