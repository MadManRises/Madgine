#include "../metalib.h"

#include "keyvaluefunction.h"

namespace Engine {

void KeyValueFunction::operator()(KeyValueReceiver &receiver, const ArgumentList &args) const
{
    mWrapper(mFunction, receiver, args);
}

}