#include "madginelib.h"
#include "refscope.h"

namespace Engine {
namespace Scripting {

RefScope::RefScope(const std::string &prototype) :
	Scope(prototype),
    mRefCount(0)
{

}

RefScope::~RefScope()
{
    assert(mRefCount == 0);
}

void RefScope::ref()
{
    ++mRefCount;
}

void RefScope::unref()
{
    assert(mRefCount > 0);
    --mRefCount;
    if (mRefCount == 0)
        delete this;
}


} // namespace Scripting
} // namespace Core

