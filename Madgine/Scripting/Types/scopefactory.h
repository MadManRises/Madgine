#pragma once

namespace Engine {
namespace Scripting {

class ScopeFactory
{
public:
    ScopeFactory(ScopeClass c);

    virtual Scope *create(Serialize::SerializeInStream &in) = 0;
};

class ScopeFactoryManager{
public:
    static ScopeFactoryManager &getSingleton();

    Scope *create(Serialize::SerializeInStream &in);

    void registerFactory(ScopeClass c, ScopeFactory *factory);

private:
    std::map<ScopeClass, ScopeFactory *> mFactories;
};

} // namespace Scripting
} // namespace Core

