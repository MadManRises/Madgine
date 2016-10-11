#include "libinclude.h"
#include "scopefactory.h"
#include "Scripting\Datatypes\Serialize\serializestream.h"

namespace Engine {
namespace Scripting {

ScopeFactoryManager &ScopeFactoryManager::getSingleton()
{
    static ScopeFactoryManager man;
    return man;
}

Scope *ScopeFactoryManager::create(Serialize::SerializeInStream &in)
{
    ScopeClass sClass;
    in >> (int&)sClass;
    auto it = mFactories.find(sClass);
    if (it == mFactories.end()) throw 0;
    return it->second->create(in);
}

void ScopeFactoryManager::registerFactory(ScopeClass c, ScopeFactory *factory)
{
    mFactories[c] = factory;
}

ScopeFactory::ScopeFactory(ScopeClass c){
    ScopeFactoryManager::getSingleton().registerFactory(c, this);
}


} // namespace Scripting
} // namespace Core

