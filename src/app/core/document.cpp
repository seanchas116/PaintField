#include <QtGui>
#include "layermodel.h"
#include "defaultdocumentio.h"

#include "document.h"

namespace PaintField
{

using namespace Malachite;

Document::Document(const QString &tempName, const QSize &size, const LayerList &layers,  QObject *parent) :
    QObject(parent),
    _size(size),
    _tempName(tempName),
    _modified(false),
    _undoStack(new QUndoStack(this)),
    _layerModel(new LayerModel(layers, this))
{
	_tileKeys = Surface::keysForRect(QRect(QPoint(), size));
	
	connect(_undoStack, SIGNAL(redoTextChanged(QString)), this, SIGNAL(redoTextChangdd(QString)));
	connect(_undoStack, SIGNAL(undoTextChanged(QString)), this, SIGNAL(undoTextChanged(QString)));
	connect(_undoStack, SIGNAL(indexChanged(int)), this, SLOT(onUndoneOrRedone()));
}

Document *Document::open(const QString &filePath, QObject *parent)
{
	return DefaultDocumentIO::open(filePath, parent);
}

bool Document::saveAs(const QString &filePath)
{
	return DefaultDocumentIO::saveAs(this, filePath);
}

void Document::setModified(bool modified)
{
	if (_modified == modified)
		return;
	_modified = modified;
	emit modifiedChanged(modified);
}

void Document::setFilePath(const QString &filePath)
{
	if (_filePath == filePath)
		return;
	_filePath = filePath;
	emit filePathChanged(filePath);
}

void Document::onUndoneOrRedone()
{
	emit modified();
	setModified(true);
}

}
