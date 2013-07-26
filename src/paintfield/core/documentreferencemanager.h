#pragma once

#include <QObject>

namespace PaintField {

class Canvas;

class DocumentReferenceManager : public QObject
{
	Q_OBJECT
public:
	explicit DocumentReferenceManager(QObject *parent = 0);
	~DocumentReferenceManager();
	
	/**
	 * Adds a canvas to the document reference list.
	 * The ownership of the document will transfer to the DocumentReferenceManager.
	 * @param canvas
	 */
	void addCanvas(Canvas *canvas);
	
	/**
	 * Remove a canvas from the document reference list and delete the document if no canvas continues to refer the document.
	 * @param canvas
	 */
	void removeCanvas(Canvas *canvas);
	
	/**
	 * If the canvas is the only one which refers the document,
	 * shows the confirmation dialog for closing the document and returns false if the user rejected.
	 * Otherwise, calls removeCanvas and returns true.
	 * @param canvas
	 * @return 
	 */
	bool tryRemoveCanvas(Canvas *canvas);
	
signals:
	
public slots:
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

