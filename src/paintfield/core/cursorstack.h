#pragma once
#include <QObject>

class QCursor;

namespace PaintField {

class CursorStack : public QObject
{
	Q_OBJECT
public:
	explicit CursorStack(QObject *parent = 0);
	~CursorStack();
	
	void setEnabled(bool enabled);
	bool isEnabled() const;
	
	void add(const QString &id, const QCursor &cursor);
	void remove(const QString &id);
	
signals:
	
public slots:
	
private:
	
	void setCursor(const QCursor &cursor);
	void updateCursor();
	
	struct Data;
	Data *d;
};

} // namespace PaintField
