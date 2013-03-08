#include <Malachite/ImageIO>
#include <Malachite/Painter>
#include <QFileInfo>
#include "drawutil.h"
#include "thumbnail.h"

#include "rasterlayer.h"

using namespace Malachite;

namespace PaintField {

RasterLayer *RasterLayer::createFromImageFile(const QString &path, QSize *imageSize)
{
	Malachite::ImageImporter importer;
	if (!importer.load(path))
		return 0;
	
	Malachite::Surface surface = importer.toSurface();
	
	QFileInfo fileInfo(path);
	
	auto layer = new RasterLayer(fileInfo.baseName());
	layer->setSurface(surface);
	
	if (imageSize)
		*imageSize = importer.size();
	
	return layer;
}

void RasterLayer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	DrawUtil::drawMLSurface(&painter, 0, 0, surface());
	
	setThumbnail(Thumbnail::createThumbnail(pixmap));
}

bool RasterLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
		case PaintField::RoleSurface:
			_surface = data.value<Surface>();
			return true;
		default:
			return super::setProperty(data, role);
	}
}

QVariant RasterLayer::property(int role) const
{
	switch (role)
	{
		case PaintField::RoleSurface:
			return QVariant::fromValue(_surface);
		default:
			return super::property(role);
	}
}

void RasterLayer::encode(QDataStream &stream) const
{
	super:encode(stream);
	stream << _surface;
}

void RasterLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	stream >> _surface;
}

void RasterLayer::saveDataFile(QDataStream &stream) const
{
	stream << _surface;
}

void RasterLayer::loadDataFile(QDataStream &stream)
{
	stream >> _surface;
}

void RasterLayer::render(Painter *painter) const
{
	painter->drawPreTransformedSurface(QPoint(), _surface);
}

QString RasterLayer::dataSuffix() const { return "surface"; }

QString RasterLayerFactory::name() const { return "raster"; }

} // namespace PaintField
