#ifdef PF_TEST

#include "autotest.h"

namespace PaintField {

AutoTest::AutoTest(QObject *parent) :
	QObject(parent)
{
}

AutoTest *AutoTest::_instance = 0;

} // namespace PaintField

#endif
