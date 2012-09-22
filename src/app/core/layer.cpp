#include "thumbnail.h"
#include "mlpainter.h"
#include "drawutil.h"

#include "layer.h"

namespace PaintField
{

using namespace Malachite;

Layer::Layer(const QString &name)
    : _parent(0),
      _name(name),
      _isLocked(false),
      _isVisible(true),
      _opacity(1.0),
      _blendMode(BlendModeNormal)
{}

Layer::Layer(const Layer &other)
    : _parent(0),
      _name(other._name),
      _isLocked(other._isLocked),
      _isVisible(other._isVisible),
      _opacity(other._opacity),
      _blendMode(other._blendMode),
      _thumbnail(other._thumbnail)
{}

Layer::~Layer()
{
	qDeleteAll(_childrenList);
}

const Layer *Layer::child(int row) const
{
	if (!contains(row))
		return 0;
	return _childrenList[row];
}

const Layer *Layer::child(const QString &name) const
{
	foreach (const Layer *e, _childrenList) {
		if (e->name() == name)
			return e;
	}
	return 0;
}

const Layer *Layer::root() const
{
	const Layer *p = this;
	forever {
		if (p->parent() == 0) return p;
		p = p->parent();
	}
}

bool Layer::isAncestorOf(const Layer *layer) const
{
	forever
	{
		if (layer == this) return true;
		if (layer == 0) return false;
		layer = layer->parent();
	}
}

bool Layer::isAncestorOfSafe(const Layer *layer) const
{
	foreach (const Layer *child, _childrenList)
	{
		if (child == layer || child->isAncestorOfSafe(layer))
			return true;
	}
	
	return false;
}

bool Layer::insertChild(int row, Layer *child)
{
	if (!insertable(row))
		return false;
	
	if (this->child(child->name()))
		return false;
	
	_childrenList.insert(row, child);
	child->_parent = this;
	
	return true;
}

bool Layer::insertChildren(int row, const LayerList &children)
{
	if (!insertable(row))
		return false;
	
	int count = children.size();
	
	for (int i = 0; i < count; ++i)
	{
		insertChild(row + i, children.at(i));
	}
	
	return true;
}

Layer *Layer::takeChild(int row)
{
	if (!contains(row))
		return 0;
	
	Layer *child = _childrenList.takeAt(row);
	child->_parent = 0;
	
	return child;
}

LayerList Layer::takeChildren()
{
	LayerList children = _childrenList;
	_childrenList.clear();
	
	foreach (Layer *child, children)
	{
		child->_parent = 0;
	}
	
	return children;
}

Layer *Layer::replaceChild(int row, Layer *child)
{
	if (!contains(row))
		return 0;
	
	Layer *oldChild = takeChild(row);
	insertChild(row, child);
	
	return oldChild;
}

bool Layer::removeChild(int row)
{
	if (!contains(row))
		return false;
	
	delete _childrenList.takeAt(row);
	
	return true;
}

Layer *Layer::cloneRecursive()
{
	Layer *dest = clone();
	
	foreach (Layer *child, _childrenList) {
		dest->appendChild(child->cloneRecursive());
	}
	
	return dest;
}

QString Layer::unduplicatedChildName(const QString &name) const
{
	if (!child(name))
		return name;
	
	int i = 0;
	forever {
		QString newName = name + " " + QString::number(++i);
		if (!child(newName))
			return newName;
	}
}

bool Layer::setProperty(const QVariant &data, int role)
{
	switch (role) {
	case PaintField::RoleName:
		_name = data.toString();
		return true;
	case PaintField::RoleVisible:
		_isVisible = data.toBool();
		return true;
	case PaintField::RoleLocked:
		_isLocked = data.toBool();
		return true;
	case PaintField::RoleThumbnail:
		_thumbnail = data.value<QPixmap>();
		return true;
	case PaintField::RoleOpacity:
		_opacity = data.toDouble();
		return true;
	case PaintField::RoleBlendMode:
		_blendMode = data.toInt();
		return true;
	default:
		return false;
	}
}

QVariant Layer::property(int role) const
{
	switch (role) {
	case PaintField::RoleName:
		return _name;
	case PaintField::RoleType:
		return type();
	case PaintField::RoleVisible:
		return _isVisible;
	case PaintField::RoleLocked:
		return _isLocked;
	case PaintField::RoleThumbnail:
		return _thumbnail;
	case PaintField::RoleOpacity:
		return _opacity;
	case PaintField::RoleBlendMode:
		return _blendMode;
	case PaintField::RoleSurface:
		return QVariant::fromValue(surface());
	default:
		return QVariant();
	}
}

void Layer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	drawMLSurface(&painter, 0, 0, surface());
	
	_thumbnail = Thumbnail::createThumbnail(pixmap);
}

void Layer::updateThumbnailRecursive(const QSize &size)
{
	updateThumbnail(size);
	foreach (Layer *child, _childrenList) {
		child->updateThumbnailRecursive(size);
	}
}

void Layer::updateDirtyThumbnailRecursive(const QSize &size)
{
	if (_isThumbnailDirty)
	{
		updateThumbnail(size);
		_isThumbnailDirty = false;
	}
	foreach (Layer *child, _childrenList) {
		child->updateDirtyThumbnailRecursive(size);
	}
}

QPointSet Layer::tileKeysRecursive() const
{
	QPointSet keys;
	keys |= tileKeys();
	foreach (const Layer *child, _childrenList) {
		keys != child->tileKeysRecursive();
	}
	return keys;
}



bool RasterLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
	case PaintField::RoleSurface:
		_surface = data.value<Malachite::Surface>();
		return true;
	default:
		return Layer::setProperty(data, role);
	}
}



void GroupLayer::updateThumbnail(const QSize &size)
{
	Layer::updateThumbnail(size);
	
	QPixmap folderIcon(":/icons/22x22/folder.png");
	if (folderIcon.isNull())
		qDebug() << "resource not found";
	
	QPixmap thumbnail = this->thumbnail();
	QPainter painter(&thumbnail);
	painter.drawPixmap(38, 38, folderIcon);
	
	setThumbnail(thumbnail);
}


}

