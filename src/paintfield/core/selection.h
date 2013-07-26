#pragma once

#include <QObject>
#include <QVariant>

namespace PaintField {

class Document;

class Selection : public QObject
{
	Q_OBJECT
	
public:
	
	enum Type
	{
		TypeNoSelection,
		TypeRect,
		TypeEllipse,
		TypePath,
		TypeSurface
	};
	
	Selection(Document *document);
	~Selection();
	
	Type type() const;
	QVariant value() const;
	bool isInverted() const;
	
public slots:
	
	void setSelection(Type type, const QVariant &value, bool inverted = false);
	
signals:
	
	void selectionChanged(Type type, const QVariant &value, bool inverted);
	
private:
	
	void setSelectionDirect(Type type, const QVariant &value, bool inverted);
	
	struct Data;
	Data *d;
};

} // namespace PaintField

