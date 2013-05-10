#ifdef PF_TEST

#include "autotest.h"
#include "testutil.h"

#include "../rasterlayer.h"
#include "../grouplayer.h"
#include "../document.h"
#include "../layerscene.h"

#include "test_layerscene.h"

namespace PaintField
{

Test_LayerScene::Test_LayerScene(QObject *parent) :
	QObject(parent)
{
}

void Test_LayerScene::test_addLayers()
{
	QList<LayerPtr> layers;
	
	for (int i = 0; i < 4; ++i)
		layers << std::make_shared<RasterLayer>("layer" + QString::number(i));
	
	auto doc = new Document("temp", QSize(400, 300), { layers[0], layers[1] });
	
	auto dir = doc->layerScene()->rootLayer();
	doc->layerScene()->addLayers({layers[2], layers[3]}, dir, 1, "test");
	
	QCOMPARE(dir->count(), 4);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer2"));
	QCOMPARE(dir->child(2)->name(), QString("layer3"));
	QCOMPARE(dir->child(3)->name(), QString("layer1"));
	
	doc->undoStack()->undo();
	
	QCOMPARE(dir->count(), 2);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer1"));
	
	doc->deleteLater();
}

void Test_LayerScene::test_removeLayers()
{
	QList<LayerPtr> layers;
	
	for (int i = 0; i < 4; ++i)
		layers << std::make_shared<RasterLayer>("layer" + QString::number(i));
	
	auto doc = new Document("temp", QSize(400, 300), layers);
	
	auto dir = doc->layerScene()->rootLayer();
	doc->layerScene()->removeLayers({ dir->child(1), dir->child(2) });
	
	QCOMPARE(dir->count(), 2);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer3"));
	
	doc->undoStack()->undo();
	
	QCOMPARE(dir->count(), 4);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer1"));
	QCOMPARE(dir->child(2)->name(), QString("layer2"));
	QCOMPARE(dir->child(3)->name(), QString("layer3"));
	
	doc->deleteLater();
}

void Test_LayerScene::test_moveLayers()
{
	auto group = std::make_shared<GroupLayer>("group");
	auto layer0 = std::make_shared<RasterLayer>("layer0");
	auto layer1 = std::make_shared<RasterLayer>("layer1");
	
	auto doc = new Document("temp", QSize(400, 300), { layer0, group, layer1 });
	auto dir = doc->layerScene()->rootLayer();
	doc->layerScene()->moveLayers( { dir->child(0), dir->child(2) }, dir->child(1), 0 );
	
	qDebug() << dir->child(0);
	
	QCOMPARE(dir->count(), 1);
	QCOMPARE(dir->child(0)->name(), QString("group"));
	QCOMPARE(dir->child(0)->count(), 2);
	QCOMPARE(dir->child(0)->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(0)->child(1)->name(), QString("layer1"));
	
	doc->undoStack()->undo();
	
	QCOMPARE(dir->count(), 3);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("group"));
	QCOMPARE(dir->child(2)->name(), QString("layer1"));
}

void Test_LayerScene::test_moveLayers_sibling()
{
	QList<LayerPtr> layers;
	
	for (int i = 0; i < 4; ++i)
		layers << std::make_shared<RasterLayer>("layer" + QString::number(i));
	
	auto doc = new Document("temp", QSize(400, 300), layers);
	auto dir = doc->layerScene()->rootLayer();
	
	doc->layerScene()->moveLayers( { dir->child(0), dir->child(2) }, dir, 0);
	
	QCOMPARE(dir->count(), 4);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer2"));
	QCOMPARE(dir->child(2)->name(), QString("layer1"));
	QCOMPARE(dir->child(3)->name(), QString("layer3"));
	
	doc->undoStack()->undo();
	
	QCOMPARE(dir->count(), 4);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer1"));
	QCOMPARE(dir->child(2)->name(), QString("layer2"));
	QCOMPARE(dir->child(3)->name(), QString("layer3"));
}

void Test_LayerScene::test_copyLayers()
{
	QList<LayerPtr> layers;
	
	for (int i = 0; i < 4; ++i)
		layers << std::make_shared<RasterLayer>("layer" + QString::number(i));
	
	auto doc = new Document("temp", QSize(400, 300), layers);
	auto dir = doc->layerScene()->rootLayer();
	
	doc->layerScene()->copyLayers( { dir->child(0), dir->child(2) }, dir, 0);
	
	QCOMPARE(dir->count(), 6);
	QCOMPARE(dir->child(0)->name(), QString("layer0 #1"));
	QCOMPARE(dir->child(1)->name(), QString("layer2 #1"));
	QCOMPARE(dir->child(2)->name(), QString("layer0"));
	QCOMPARE(dir->child(3)->name(), QString("layer1"));
	QCOMPARE(dir->child(4)->name(), QString("layer2"));
	QCOMPARE(dir->child(5)->name(), QString("layer3"));
	
	doc->undoStack()->undo();
	
	QCOMPARE(dir->count(), 4);
	QCOMPARE(dir->child(0)->name(), QString("layer0"));
	QCOMPARE(dir->child(1)->name(), QString("layer1"));
	QCOMPARE(dir->child(2)->name(), QString("layer2"));
	QCOMPARE(dir->child(3)->name(), QString("layer3"));
}

void Test_LayerScene::test_setLayerProperty()
{
	auto doc = new Document("temp", QSize(400, 300), {std::make_shared<RasterLayer>("layer")});
	
	auto dir = doc->layerScene()->rootLayer();
	auto layer = dir->child(0);
	doc->layerScene()->setLayerProperty(layer, "newname", RoleName);
	QCOMPARE(layer->name(), QString("newname"));
	doc->undoStack()->undo();
	QCOMPARE(layer->name(), QString("layer"));
	
	doc->layerScene()->setLayerProperty(layer, true, RoleLocked);
	QCOMPARE(layer->isLocked(), true);
	
	doc->layerScene()->setLayerProperty(layer, "newname", RoleName);
	QCOMPARE(layer->name(), QString("layer"));
}

PF_ADD_TESTCLASS(Test_LayerScene)

}

#endif
