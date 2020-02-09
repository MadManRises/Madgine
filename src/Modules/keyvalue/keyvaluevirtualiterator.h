#pragma once

#include "../generic/virtualiterator.h"

namespace Engine {

using KeyValueVirtualIterator = VirtualIterator<std::pair<ValueType, ValueTypeRef>>;

}