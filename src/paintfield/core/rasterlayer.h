#pragma once
#include "layer.h"

namespace PaintField {

class RasterLayer : public Layer
{
public:
	
	typedef Layer super;
	
	RasterLayer(const QString &name = QString()) : Layer(name) {}
	
	Layer *createAnother() const override { return new RasterLayer(); }
	
	Malachite::Surface surface() const { return _surface; }
	void setSurface(const Malachite::Surface &surface) { _surface = surface; setThumbnailDirty(true); }
	
	Malachite::Surface *psurface() { return &_surface; }
	
	QPointSet tileKeys() const override { return _surface.keys(); }
	
	bool includes(const QPoint &pos, int margin) const override;
	bool isGraphicallySelectable() const override;
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void updateThumbnail(const QSize &size) override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	bool hasDataToSave() const override { return true; }
	void saveDataFile(QDataStream &stream) const override;
	void loadDataFile(QDataStream &stream) override;
	QString dataSuffix() const override;
	
	void render(Malachite::Painter *painter) const override;
	
	static RasterLayer *createFromImageFile(const QString &path, QSize *imageSize = 0);
	static RasterLayer *createFromImage(const QImage &image);
	
private:
	
	Malachite::Surface _surface;
};

class RasterLayerFactory : public LayerFactory
{
public:
	
	RasterLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	Layer *create() const override { return new RasterLayer(); }
	const ::std::type_info &typeInfo() const override { return typeid(RasterLayer); }
};

} // namespace PaintField
