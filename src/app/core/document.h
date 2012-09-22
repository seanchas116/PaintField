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
	
	Document(const QString &tempName, const QSize &size, const LayerList &layers, QObject *parent = 0);
	Document(const QString &tempName, const QSize &size, Layer *layer, QObject *parent = 0) :
		Document(tempName, size, Malachite::listFromValue(layer), parent) {}
	
	static Document *open(const QString &filePath, QObject *parent = 0);
	bool saveAs(const QString &filePath);
	bool save() { return saveAs(filePath()); }
	
	QSize size() const { return _size; }
	int width() const { return _size.width(); }
	int height() const { return _size.height(); }
	bool isModified() const { return _modified; }
	bool isNew() const { return _filePath.isEmpty(); }
	QString filePath() const { return _filePath; }
	QString fileName() const { return _filePath.isEmpty() ? _tempName : _filePath.section('/', -1); }
	QString tempName() const { return _tempName; }
	int tileXCount() const { return _size.width() / Malachite::Surface::TileSize + 1; }
	int tileYCount() const { return _size.height() / Malachite::Surface::TileSize + 1; }
	QPointSet tileKeys() const { return _tileKeys; }
	bool canRedo() const { return _undoStack->canRedo(); }
	bool canUndo() const { return _undoStack->canUndo(); }
	
	QUndoStack *undoStack() { return _undoStack; }
	LayerModel *layerModel() { return _layerModel; }
	
	void setModified(bool modified);
	void setFilePath(const QString &filePath);
	
signals:
	
	void modified();
	void modifiedChanged(bool modified);
	void filePathChanged(const QString &filePath);
	
	void undoTextChanged(const QString &name);
	void redoTextChangdd(const QString &name);
	
public slots:
	
	void undo() { _undoStack->undo(); }
	void redo() { _undoStack->redo(); }
	
protected:
	
private slots:
	
	void onUndoneOrRedone();
	
private:
	
	QSize _size;
	QPointSet _tileKeys;
	QString _filePath;
	QString _tempName;	// like "untitled"
	bool _modified;
	QUndoStack *_undoStack;
	
	LayerModel *_layerModel;
};

}

#endif // FSDOCUMENTMODEL_H
