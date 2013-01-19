#include "memorizablesplitter.h"

namespace PaintField {

void MemorizableSplitter::memorizeSizes()
{
	_memorizedSizes = sizes();
}

void MemorizableSplitter::restoreSizes()
{
	if (_memorizedSizes.size())
		setSizes(_memorizedSizes);
}

} // namespace PaintField
