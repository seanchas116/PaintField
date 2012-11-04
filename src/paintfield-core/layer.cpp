#include <Malachite/Painter>

#include "thumbnail.h"
#include "drawutil.h"

#include "layer.h"

namespace PaintField
{

using namespace Malachite;

Layer::Layer(const QString &name)
    : _name(name),
      _blendMode(BlendModeNormal)
{}

Layer::Layer(const Layer &other)
    : _name(other._name),
      _isLocked(other._isLocked),
      _isVisible(other._isVisible),
      _opacity(other._opacity),
      _blendMode(other._blendMode),
      _thumbnail(other._thumbnail)
{}

Layer::~Layer()
{
	qDeleteAll(_children);
}

const Layer *Layer::child(int row) const
{
	if (!contains(row))
		return 0;
	return _children[row];
}

const Layer *Layer::child(const QString &name) const
{
	for (const Layer *child : _children)
	{
		if (child->name() == name)
			return child;
	}
	return 0;
}

const Layer *Layer::root() const
{
	const Layer *layer = this;
	
	forever
	{
		if (layer->parent() == 0) return layer;
		layer = layer->parent();
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
	if (this == layer)
		return true;
	
	for (const Layer *child : _children)
	{
		if (child->isAncestorOfSafe(layer))
			return true;
	}
	
	return false;
}

bool Layer::insertChild(int row, Layer *child)
{
	if (!insertable(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return false;
	}
	
	_children.insert(row, child);
	child->_parent = this;
	
	return true;
}

bool Layer::insertChildren(int row, const LayerList &children)
{
	if (!insertable(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return false;
	}
	
	int count = children.size();
	
	for (int i = 0; i < count; ++i)
		insertChild(row + i, children.at(i));
	
	return true;
}

Layer *Layer::takeChild(int row)
{
	if (!contains(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return 0;
	}
	
	Layer *child = _children.takeAt(row);
	child->_parent = 0;
	
	return child;
}

LayerList Layer::takeChildren()
{
	LayerList children = _children;
	_children.clear();
	
	for (Layer *child : children)
		child->_parent = 0;
	
	return children;
}

Layer *Layer::replaceChild(int row, Layer *child)
{
	if (!contains(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return 0;
	}
	
	Layer *oldChild = takeChild(row);
	insertChild(row, child);
	
	return oldChild;
}

bool Layer::removeChild(int row)
{
	if (!contains(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return false;
	}
	
	delete _children.takeAt(row);
	
	return true;
}

bool Layer::shiftChildren(int start, int end, int shiftCount)
{
	if (start == end)
		return true;
	
	if (contains(start) && contains(end))
	{
		shiftContainer(_children, start, end, shiftCount);
		return true;
	}
	
	qWarning() << Q_FUNC_INFO << ": invaild row";
	return false;
}

Layer *Layer::cloneRecursive()
{
	Layer *dest = clone();
	
	for (Layer *child : _children)
		dest->appendChild(child->cloneRecursive());
	
	return dest;
}

QStringList Layer::childNames() const
{
	QStringList list;
	
	for (Layer *child : _children)
		list << child->name();
	
	return list;
}

bool Layer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
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
	switch (role)
	{
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
	for (Layer *child : _children)
		child->updateThumbnailRecursive(size);
}

void Layer::updateDirtyThumbnailRecursive(const QSize &size)
{
	if (_isThumbnailDirty)
	{
		updateThumbnail(size);
		_isThumbnailDirty = false;
	}
	
	for (Layer *child : _children)
		child->updateDirtyThumbnailRecursive(size);
}

QPointSet Layer::tileKeysRecursive() const
{
	QPointSet keys;
	keys |= tileKeys();
	
	for (const Layer *child : _children)
		keys != child->tileKeysRecursive();
	
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

