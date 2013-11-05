#ifndef PAINTFIELD_CUSTOMBRUSHEDITOR_H
#define PAINTFIELD_CUSTOMBRUSHEDITOR_H

#include "brusheditorview.h"

namespace PaintField {

class ObservableVariantMap;

class CustomBrushEditor : public QWidget
{
	Q_OBJECT
public:
	explicit CustomBrushEditor(ObservableVariantMap *parameters, QWidget *parent = 0);
	~CustomBrushEditor();

signals:

public slots:

private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

#endif // PAINTFIELD_CUSTOMBRUSHEDITOR_H
