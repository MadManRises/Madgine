#pragma once

#include "Generic/container/virtualrange.h"

namespace Engine {

using KeyValueVirtualAssociativeIterator = VirtualIterator<KeyValuePair>;
using KeyValueVirtualAssociativeRange = VirtualRange<KeyValuePair, Functor_to_KeyValuePair<true>>;
using KeyValueVirtualSequenceIterator = VirtualIterator<ValueTypeRef>;
using KeyValueVirtualSequenceRange = VirtualRange<ValueTypeRef, Functor_to_ValueTypeRef<true>>;


}