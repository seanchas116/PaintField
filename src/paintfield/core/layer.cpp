#include <QFileInfo>

#include <Malachite/Painter>

#include "thumbnail.h"
#include "drawutil.h"

#include "layer.h"

namespace PaintField
{

using namespace Malachite;

Layer::Layer(const QString &name)
    : _name(name),
      _blendMode(BlendMode::Normal)
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

bool Layer::insert(int row, Layer *child)
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

bool Layer::insert(int row, const LayerList &children)
{
	if (!insertable(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return false;
	}
	
	int count = children.size();
	
	for (int i = 0; i < count; ++i)
		insert(row + i, children.at(i));
	
	return true;
}

Layer *Layer::take(int row)
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

LayerList Layer::takeAll()
{
	LayerList children = _children;
	_children.clear();
	
	for (Layer *child : children)
		child->_parent = 0;
	
	return children;
}

Layer *Layer::replace(int row, Layer *child)
{
	if (!contains(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return 0;
	}
	
	Layer *oldChild = take(row);
	insert(row, child);
	
	return oldChild;
}

bool Layer::remove(int row)
{
	if (!contains(row))
	{
		qWarning() << Q_FUNC_INFO << ": invaild row";
		return false;
	}
	
	delete _children.takeAt(row);
	
	return true;
}

bool Layer::shift(int start, int end, int shiftCount)
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

Layer *Layer::clone() const
{
	auto layer = this->createAnother();
	QByteArray array;
	
	{
		QDataStream stream(&array, QIODevice::WriteOnly);
		this->encode(stream);
	}
	
	{
		QDataStream stream(&array, QIODevice::ReadOnly);
		layer->decode(stream);
	}
	
	return layer;
}

Layer *Layer::cloneRecursive() const
{
	Layer *dest = clone();
	
	for (Layer *child : _children)
		dest->append(child->cloneRecursive());
	
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
		case PaintField::RoleVisible:
			return _isVisible;
		case PaintField::RoleLocked:
			return _isLocked;
		case PaintField::RoleThumbnail:
			return _thumbnail;
		case PaintField::RoleOpacity:
			return _opacity;
		case PaintField::RoleBlendMode:
			return _blendMode.toInt();
		default:
			return QVariant();
	}
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

const Layer *Layer::descendantAt(const QPoint &pos) const
{
	for (const Layer *child : _children)
	{
		if (child->includes(pos))
			return child;
		
		auto descendant = child->descendantAt(pos);
		if (descendant)
			return descendant;
	}
	return 0;
}

QPointSet Layer::tileKeysRecursive() const
{
	QPointSet keys;
	keys |= tileKeys();
	
	for (const Layer *child : _children)
		keys |= child->tileKeysRecursive();
	
	return keys;
}

QVariantMap Layer::saveProperies() const
{
	QVariantMap map;
	map["name"] = _name;
	map["visible"] = _isVisible;
	map["locked"] = _isLocked;
	map["opacity"] = _opacity;
	map["blendMode"] = _blendMode.toString();
	return map;
}

void Layer::loadProperties(const QVariantMap &map)
{
	_name = map["name"].toString();
	_isVisible = map["visible"].toBool();
	_isLocked = map["locked"].toBool();
	_opacity = map["opacity"].toDouble();
	_blendMode = map["blendMode"].toString();
}

void Layer::encode(QDataStream &stream) const
{
	stream << _name << _isVisible << _isLocked << _opacity << _blendMode.toInt() << _thumbnail << _isThumbnailDirty;
}

void Layer::decode(QDataStream &stream)
{
	int blend;
	stream >> _name >> _isVisible >> _isLocked >> _opacity >> blend >> _thumbnail >> _isThumbnailDirty;
	_blendMode = blend;
}


}

