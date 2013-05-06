#ifndef FSDOCUMENTMODEL_H
#define FSDOCUMENTMODEL_H

#include <QUndoStack>

#include "layer.h"

namespace PaintField {

class LayerScene;
class Selection;

class Document : public QObject
{
	Q_OBJECT
	
public:
	
	/**
	 * Constructs a document.
	 * @param tempName The temporary file name which will be used until the document is saved
	 * @param size The size of the document
	 * @param layers The layers first added to the document
	 * @param parent The QObject parent
	 */
	Document(const QString &tempName, const QSize &size, const QList<LayerPtr> &layers, QObject *parent = 0);
	~Document();
	
	QSize size() const;
	int width() const { return size().width(); }
	int height() const { return size().height(); }
	
	/**
	 * @return Wheter the document is modified (unsaved)
	 */
	bool isModified() const;
	
	/**
	 * @return Whether he document is new and has never saved
	 */
	bool isNew() const;
	
	/**
	 * @return The full file path
	 */
	QString filePath() const;
	
	/**
	 * @return The last section of the file path if the document is saved, otherwise the temporary name
	 */
	QString fileName() const;
	
	/**
	 * @return The temporary name, like "Untitled"
	 */
	QString tempName() const;
	
	/**
	 * @return ceil( width / Surface tile size )
	 */
	int tileXCount() const;
	
	/**
	 * @return ceil( height / Surface tile size )
	 */
	int tileYCount() const;
	
	/**
	 * @return { (0, 0), (0, 1), ... , (tileXCount(), tileYCount()) }
	 */
	QPointSet tileKeys() const;
	
	QUndoStack *undoStack();
	
	LayerScene *layerScene();
	
	Selection *selection();
	
	void setModified(bool modified);
	void setFilePath(const QString &filePath);
	
signals:
	
	void modified();
	void modifiedChanged(bool modified);
	void filePathChanged(const QString &filePath);
	void fileNameChanged(const QString &fileName);
	
public slots:
	
protected:
	
private slots:
	
	void onUndoneOrRedone();
	
private:
	
	struct Data;
	Data *d;
};

}

#endif // FSDOCUMENTMODEL_H
