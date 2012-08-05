#include "fsthumbnail.h"
#include "mlpainter.h"
#include "fsdocumentmodel.h"
#include "fsdrawutil.h"

#include "fslayer.h"

FSLayer::FSLayer(const QString &name)
    : _parent(0),
      _name(name),
      _isLocked(false),
      _isVisible(true),
      _opacity(1.0),
      _blendMode(MLGlobal::BlendModeNormal)
{}

FSLayer::FSLayer(const FSLayer &other)
    : _parent(0),
      _name(other._name),
      _isLocked(other._isLocked),
      _isVisible(other._isVisible),
      _opacity(other._opacity),
      _blendMode(other._blendMode),
      _thumbnail(other._thumbnail)
{}

FSLayer::~FSLayer()
{
	qDeleteAll(_childrenList);
}

const FSLayer *FSLayer::child(int row) const
{
	if (!contains(row))
		return 0;
	return _childrenList[row];
}

const FSLayer *FSLayer::child(const QString &name) const
{
	foreach (const FSLayer *e, _childrenList) {
		if (e->name() == name)
			return e;
	}
	return 0;
}

const FSLayer *FSLayer::root() const
{
	const FSLayer *p = this;
	forever {
		if (p->parent() == 0) return p;
		p = p->parent();
	}
}

bool FSLayer::isAncestorOf(const FSLayer *layer) const
{
	forever
	{
		if (layer == this) return true;
		if (layer == 0) return false;
		layer = layer->parent();
	}
}

bool FSLayer::insertChild(int row, FSLayer *child)
{
	if (!insertable(row))
		return false;
	
	if (this->child(child->name()))
		return false;
	
	_childrenList.insert(row, child);
	child->_parent = this;
	
	return true;
}

FSLayer *FSLayer::takeChild(int row)
{
	if (!contains(row))
		return 0;
	
	FSLayer *child = _childrenList.takeAt(row);
	child->_parent = 0;
	
	return child;
}

FSLayer *FSLayer::replaceChild(int row, FSLayer *child)
{
	if (!contains(row))
		return 0;
	
	FSLayer *oldChild = takeChild(row);
	insertChild(row, child);
	
	return oldChild;
}

bool FSLayer::removeChild(int row)
{
	if (!contains(row))
		return false;
	
	delete _childrenList.takeAt(row);
	
	return true;
}

FSLayer *FSLayer::cloneRecursive()
{
	FSLayer *dest = clone();
	
	foreach (FSLayer *child, _childrenList) {
		dest->appendChild(child->cloneRecursive());
	}
	
	return dest;
}

QString FSLayer::unduplicatedChildName(const QString &name) const
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

bool FSLayer::setProperty(const QVariant &data, int role)
{
	switch (role) {
	case FSGlobal::RoleName:
		_name = data.toString();
		return true;
	case FSGlobal::RoleVisible:
		_isVisible = data.toBool();
		return true;
	case FSGlobal::RoleLocked:
		_isLocked = data.toBool();
		return true;
	case FSGlobal::RoleThumbnail:
		_thumbnail = data.value<QPixmap>();
		return true;
	case FSGlobal::RoleOpacity:
		_opacity = data.toDouble();
		return true;
	case FSGlobal::RoleBlendMode:
		_blendMode = data.value<MLBlendMode>();
		return true;
	default:
		return false;
	}
}

QVariant FSLayer::property(int role) const
{
	switch (role) {
	case FSGlobal::RoleName:
		return _name;
	case FSGlobal::RoleType:
		return type();
	case FSGlobal::RoleVisible:
		return _isVisible;
	case FSGlobal::RoleLocked:
		return _isLocked;
	case FSGlobal::RoleThumbnail:
		return _thumbnail;
	case FSGlobal::RoleOpacity:
		return _opacity;
	case FSGlobal::RoleBlendMode:
		return QVariant::fromValue(_blendMode);
	case FSGlobal::RoleSurface:
		return QVariant::fromValue(surface());
	default:
		return QVariant();
	}
}

void FSLayer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	fsDrawMLSurface(&painter, 0, 0, surface());
	
	_thumbnail = FSThumbnail::createThumbnail(pixmap);
}

void FSLayer::updateThumbnailRecursive(const QSize &size)
{
	updateThumbnail(size);
	foreach (FSLayer *child, _childrenList) {
		child->updateThumbnailRecursive(size);
	}
}

void FSLayer::updateDirtyThumbnailRecursive(const QSize &size)
{
	if (_isThumbnailDirty)
	{
		updateThumbnail(size);
		_isThumbnailDirty = false;
	}
	foreach (FSLayer *child, _childrenList) {
		child->updateDirtyThumbnailRecursive(size);
	}
}

QPointSet FSLayer::tileKeysRecursive() const
{
	QPointSet keys;
	keys |= tileKeys();
	foreach (const FSLayer *child, _childrenList) {
		keys != child->tileKeysRecursive();
	}
	return keys;
}


