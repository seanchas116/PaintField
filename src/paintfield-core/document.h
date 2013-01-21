#ifndef FSDOCUMENTMODEL_H
#define FSDOCUMENTMODEL_H

#include <QUndoStack>

#include "layermodel.h"
#include "layer.h"

namespace PaintField {

class LayerModel;

class Document : public QObject
{
	Q_OBJECT
	friend class FSDocumentCommand;
	
public:
	
	/**
	 * Constructs a document.
	 * @param tempName The temporary file name which will be used until the document is saved
	 * @param size The size of the document
	 * @param layers The layers first added to the document
	 * @param parent The QObject parent
	 */
	Document(const QString &tempName, const QSize &size, const LayerList &layers, QObject *parent = 0);
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
	 * @return The full path
	 */
	QString filePath() const;
	QString fileName() const;
	QString tempName() const;
	int tileXCount() const { return width() / Malachite::Surface::TileSize + 1; }
	int tileYCount() const { return height() / Malachite::Surface::TileSize + 1; }
	QPointSet tileKeys() const;
	
	QUndoStack *undoStack();
	LayerModel *layerModel();
	
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
	
	class Data;
	Data *d;
};

}

#endif // FSDOCUMENTMODEL_H
