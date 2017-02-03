#include "interfaceslib.h"
#include "refscope.h"
#include "refscopetoplevelserializableunit.h"

namespace Engine {
namespace Scripting {

RefScope::RefScope(RefScopeTopLevelSerializableUnit *topLevel) :
    mRefCount(0),	
	mTopLevel(topLevel)
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
	if (mRefCount == 0) {
		mTopLevel->removeRefScope(this);
	}
}


} // namespace Scripting
} // namespace Core

