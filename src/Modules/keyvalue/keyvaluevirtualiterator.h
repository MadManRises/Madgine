#pragma once

#include "../generic/container/virtualiterator.h"

namespace Engine {

using KeyValueVirtualIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualRange = VirtualRange<KeyValuePair>;

}