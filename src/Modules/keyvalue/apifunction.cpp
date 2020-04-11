#include "../moduleslib.h"

#include "apifunction.h"

#include "functiontable.h"

namespace Engine {

size_t ApiFunction::argumentsCount() const
{
    return mTable->mArgumentsCount;
}

}