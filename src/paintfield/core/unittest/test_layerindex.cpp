#ifdef PF_TEST

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "autotest.h"
#include "../grouplayer.h"

#include "test_layerindex.h"

using namespace boost;

namespace PaintField {

Test_LayerIndex::Test_LayerIndex(QObject *parent) :
	QObject(parent)
{
}

void Test_LayerIndex::test()
{
	auto layer = new GroupLayer();
	
	layer->append(new GroupLayer());
	layer->append(new GroupLayer());
	
	LayerIndex index = layer;
	
	QCOMPARE(index.child(1).layer(), layer->child(1));
	
	delete layer;
	
	QCOMPARE(index.layer(), static_cast<const Layer *>(0));
}

PF_ADD_TESTCLASS(Test_LayerIndex)

} // namespace PaintField

#endif
