#include <QFileInfo>

#include <Malachite/Painter>

#include "layerfactorymanager.h"

#include "thumbnail.h"
#include "drawutil.h"

#include "layer.h"

namespace PaintField
{

using namespace Malachite;

Layer::Layer(const QString &name)
    : _name(name),
      _blendMode(BlendMode::Normal)
{
}

Layer::~Layer()
{
}

LayerConstPtr Layer::constChild(int index) const
{
	if (!contains(index))
		return nullptr;
	return _children[index];
}

LayerConstPtr Layer::constRoot() const
{
	auto layer = shared_from_this();
	
	forever
	{
		if (layer->parent() == nullptr)
			return layer;
		layer = layer->parent();
	}
}

bool Layer::isAncestorOf(const LayerConstPtr &layer) const
{
	auto l = layer;
	
	forever
	{
		if (l.get() == this)
			return true;
		if (l == nullptr)
			return false;
		l = l->parent();
	}
}

bool Layer::isAncestorOfSafe(const LayerConstPtr &layer) const
{
	if (this == layer.get())
		return true;
	
	for (const auto &child : _children)
	{
		if (child->isAncestorOfSafe(layer))
			return true;
	}
	
	return false;
}

bool Layer::insert(int index, const LayerPtr &child)
{
	if (!insertable(index))
	{
		PAINTFIELD_WARNING << "invalid row";
		return false;
	}
	
	_children.insert(index, child);
	child->_parent = shared_from_this();
	
	return true;
}

bool Layer::insert(int index, const QList<LayerPtr> &children)
{
	if (!insertable(index))
	{
		PAINTFIELD_WARNING << "invalid row";
		return false;
	}
	
	int count = children.size();
	
	for (int i = 0; i < count; ++i)
		insert(index + i, children.at(i));
	
	return true;
}

LayerPtr Layer::take(int row)
{
	if (!contains(row))
	{
		PAINTFIELD_WARNING << "invalid row";
		return nullptr;
	}
	
	auto child = _children.takeAt(row);
	child->_parent = std::weak_ptr<Layer>();
	return child;
}

QList<LayerPtr> Layer::takeAll()
{
	auto children = _children;
	_children.clear();
	
	for (const auto &child : children)
		child->_parent = std::weak_ptr<Layer>();
	
	return children;
}

LayerPtr Layer::clone() const
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

LayerPtr Layer::cloneRecursive() const
{
	auto dest = clone();
	
	for (const auto &child : _children)
		dest->append(child->cloneRecursive());
	
	return dest;
}

QStringList Layer::childNames() const
{
	QStringList list;
	
	for (const auto &child : _children)
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
	for (const auto &child : _children)
		child->updateThumbnailRecursive(size);
}

void Layer::updateDirtyThumbnailRecursive(const QSize &size)
{
	if (_isThumbnailDirty)
	{
		updateThumbnail(size);
		_isThumbnailDirty = false;
	}
	
	for (const auto &child : _children)
		child->updateDirtyThumbnailRecursive(size);
}

LayerConstPtr Layer::descendantAt(const QPoint &pos, int margin) const
{
	for (const auto &child : _children)
	{
		if (child->includes(pos, margin))
			return child;
		
		auto descendant = child->descendantAt(pos, margin);
		if (descendant)
			return descendant;
	}
	return 0;
}

QPointSet Layer::tileKeysRecursive() const
{
	QPointSet keys;
	keys |= tileKeys();
	
	for (const auto &child : _children)
		keys |= child->tileKeysRecursive();
	
	return keys;
}

void Layer::encodeRecursive(QDataStream &stream) const
{
	auto typeName = layerFactoryManager()->nameForTypeInfo(typeid(*this));
	stream << typeName;
	encode(stream);
	
	stream << _children.size();
	for (auto child : _children)
		child->encodeRecursive(stream);
}

LayerPtr Layer::decodeRecursive(QDataStream &stream)
{
	QString typeName;
	stream >> typeName;
	
	auto layer = layerFactoryManager()->createLayer(typeName);
	
	if (!layer)
		return 0;
	
	layer->decode(stream);
	
	int count;
	stream >> count;
	
	for (int i = 0; i < count; ++i)
	{
		auto child = decodeRecursive(stream);
		if (!child)
			return layer;
		
		layer->append(child);
	}
	
	return layer;
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

QDebug operator<<(QDebug debug, const PaintField::LayerConstPtr &layer)
{
	debug.nospace() << "(" << layer.get() << ", " << (layer ? layer->name() : QString()) << ")";
	return debug.space();
}

