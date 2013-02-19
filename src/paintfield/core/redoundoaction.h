#pragma once
#include <QAction>

namespace PaintField {

class RedoUndoAction : public QAction
{
	Q_OBJECT
public:
	explicit RedoUndoAction(QObject *parent);
	~RedoUndoAction();
	
	void setPrefix(const QString &text);
	QString prefix() const;
	
	QString description() const;
	
signals:
	
public slots:
	
	void setDescription(const QString &text);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
