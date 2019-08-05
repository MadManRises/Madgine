#pragma once

#include "uniquecomponentregistry.h"

#ifndef STATIC_BUILD

#include "../reflection/classname.h"
#include "../keyvalue/metatable.h"

#include "indexholder.h"

#include "../reflection/decay.h"

namespace Engine {

template <class _Base, class _Ty>
struct UniqueComponentCollector {
    typedef _Base Base;
    typedef _Ty Ty;
    typedef Collector_F<Base, Ty> F;
    typedef UniqueComponentRegistry<Base, Ty> Registry;

    UniqueComponentCollector()
    {
        mInfo.mRegistryInfo = &typeInfo<Registry>();
        mInfo.mBaseInfo = &typeInfo<Base>();
        mInfo.mBinary = &Plugins::PLUGIN_LOCAL(binaryInfo);
        Registry::sInstance().addCollector(&mInfo);
    }
    UniqueComponentCollector(const UniqueComponentCollector &) = delete;
    ~UniqueComponentCollector()
    {
        Registry::sInstance().removeCollector(&mInfo);
    }

    void operator=(const UniqueComponentCollector &) = delete;

    static UniqueComponentCollector &sInstance();

    size_t size() const
    {
        return sInstance().mInfo.mComponents.size();
    }

private:
    CollectorInfo mInfo;

    template <class T>
    static size_t registerComponent()
    {

        LOG("Registering Component: " << typeName<T>());
        sInstance().mInfo.mComponents.emplace_back(reinterpret_cast<Collector_F<void, void *>>(&createComponent<T, Base, Ty>));
        sInstance().mInfo.mElementInfos.push_back(&typeInfo<T>());
        sInstance().mInfo.mElementTables.push_back(std::is_base_of_v<Engine::ScopeBase, T> ? &table<decayed_t<T>>() : nullptr);
        return sInstance().mInfo.mComponents.size() - 1;
    }

    static void unregisterComponent(size_t i)
    {
        sInstance().mInfo.mComponents[i] = nullptr;
        sInstance().mInfo.mElementInfos[i] = nullptr;
        sInstance().mInfo.mElementTables[i] = nullptr;
    }

    static size_t &baseIndex()
    {
        return sInstance().mInfo.mBaseIndex;
    }

public:
    template <class T>
    class ComponentRegistrator : public IndexHolder {
    public:
        ComponentRegistrator()
            : mBaseIndex(baseIndex())
        {
            mIndex = registerComponent<T>();
        }

        ~ComponentRegistrator()
        {
            unregisterComponent(mIndex);
            mIndex = -1;
        }

        size_t index() override
        {
            return mIndex + mBaseIndex;
        }

    private:
        size_t mIndex = -1;
        //Make it a member to prevent problems through weak symbols during link time
        size_t &mBaseIndex;
    };
};

template <class _Base, class _Ty>
UniqueComponentCollector<_Base, _Ty> &UniqueComponentCollector<_Base, _Ty>::sInstance()
{
    static UniqueComponentCollector dummy;
    return dummy;
}

}

#else

namespace Engine {

template <class _Base, class _Ty>
struct UniqueComponentCollector {
    typedef _Base Base;
    typedef _Ty Ty;
    typedef UniqueComponentRegistry<Base, Ty> Registry;
};

}

#endif

//#define RegisterCollector(collector) RegisterType(collector::Registry)
