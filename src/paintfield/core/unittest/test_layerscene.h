#pragma once

#ifdef PF_TEST

#include <QObject>

namespace PaintField
{

class Test_LayerScene : public QObject
{
	Q_OBJECT
public:
	explicit Test_LayerScene(QObject *parent = 0);
	
private slots:
	
	void test_addLayers();
	void test_removeLayers();
	void test_moveLayers();
	void test_moveLayers_sibling();
	void test_copyLayers();
	
	void test_setLayerProperty();
};

}

#endif
