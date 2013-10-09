#ifndef PAINTFIELD_CUSTOMBRUSHEDITOR_H
#define PAINTFIELD_CUSTOMBRUSHEDITOR_H

#include "brusheditorview.h"

namespace PaintField {

class CustomBrushEditor : public BrushEditor
{
	Q_OBJECT
public:
	explicit CustomBrushEditor(QWidget *parent = 0);
	~CustomBrushEditor();

	void setSettings(const QVariantMap &settings) override;

signals:

public slots:

private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField

#endif // PAINTFIELD_CUSTOMBRUSHEDITOR_H
