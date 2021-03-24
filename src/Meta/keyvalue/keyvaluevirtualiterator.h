#pragma once

#include "Generic/container/virtualrange.h"

namespace Engine {

using KeyValueVirtualIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualRange = VirtualRange<KeyValuePair, Functor_to_KeyValuePair<true>>;

}