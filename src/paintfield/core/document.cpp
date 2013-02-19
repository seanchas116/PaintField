#include <QtGui>
#include "layermodel.h"

#include "document.h"

namespace PaintField
{

using namespace Malachite;

struct Document::Data
{
	QSize size;
	QPointSet tileKeys;
	int tileXCount, tileYCount;
	QString filePath;
	QString tempName;	// like "untitled"
	bool modified = false;
	QUndoStack *undoStack = 0;
	
	LayerModel *layerModel = 0;
};

Document::Document(const QString &tempName, const QSize &size, const LayerList &layers,  QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->size = size;
	d->tempName = tempName;
	d->undoStack = new QUndoStack(this);
	d->layerModel = new LayerModel(layers, this);
	d->tileXCount = ceil(double(size.width()) / double(Surface::tileWidth()));
	d->tileXCount = ceil(double(size.height()) / double(Surface::tileWidth()));
	d->tileKeys = Surface::rectToKeys(QRect(QPoint(), size));
	
	connect(d->undoStack, SIGNAL(indexChanged(int)), this, SLOT(onUndoneOrRedone()));
}

Document::~Document()
{
	delete d;
}

QSize Document::size() const
{
	return d->size;
}

bool Document::isModified() const
{
	return d->modified;
}

bool Document::isNew() const
{
	return d->filePath.isEmpty();
}

QString Document::filePath() const
{
	return d->filePath;
}

QString Document::fileName() const
{
	return d->filePath.isEmpty() ? d->tempName : d->filePath.section('/', -1);
}

QString Document::tempName() const
{
	return d->tempName;
}

int Document::tileXCount() const
{
	return d->tileXCount;
}

int Document::tileYCount() const
{
	return d->tileYCount;
}

QPointSet Document::tileKeys() const
{
	return d->tileKeys;
}

QUndoStack *Document::undoStack()
{
	return d->undoStack;
}

LayerModel *Document::layerModel()
{
	return d->layerModel;
}

void Document::setModified(bool modified)
{
	if (d->modified == modified)
		return;
	d->modified = modified;
	emit modifiedChanged(modified);
}

void Document::setFilePath(const QString &filePath)
{
	if (d->filePath == filePath)
		return;
	d->filePath = filePath;
	emit filePathChanged(filePath);
	emit fileNameChanged(fileName());
}

void Document::onUndoneOrRedone()
{
	emit modified();
	setModified(true);
}

}
