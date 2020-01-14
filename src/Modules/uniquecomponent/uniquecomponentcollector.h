#pragma once

#include "uniquecomponentregistry.h"

#if ENABLE_PLUGINS

#    include "../keyvalue/metatable.h"
#    include "../reflection/classname.h"

#    include "indexholder.h"

#    include "../reflection/decay.h"

#    include "../generic/derive.h"

namespace Engine {

DERIVE_TYPEDEF(VBase)

template <typename _Base, typename... _Ty>
struct UniqueComponentCollector {
    typedef _Base Base;
    typedef std::tuple<_Ty...> Ty;
    typedef Collector_F<Base, _Ty...> F;
    typedef UniqueComponentRegistry<Base, _Ty...> Registry;

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

    template <typename T>
    static size_t registerComponent()
    {

        LOG("Registering Component: " << typeName<T>());
        sInstance().mInfo.mComponents.emplace_back(reinterpret_cast<CollectorInfo::ComponentType>(&createComponent<T, Base, _Ty...>));
        std::vector<const TypeInfo *> elementInfos;
        elementInfos.push_back(&typeInfo<T>());
        if constexpr (has_typedef_VBase_v<T>) {
            elementInfos.push_back(&typeInfo<typename T::VBase>());
        }
        sInstance().mInfo.mElementInfos.emplace_back(std::move(elementInfos));
        sInstance().mInfo.mElementTables.push_back(std::is_base_of_v<Engine::ScopeBase, T> ? &table<decayed_t<T>>() : nullptr);
        return sInstance().mInfo.mComponents.size() - 1;
    }

    static void unregisterComponent(size_t i)
    {
        sInstance().mInfo.mComponents[i] = nullptr;
        sInstance().mInfo.mElementInfos[i].clear();
        sInstance().mInfo.mElementTables[i] = nullptr;
    }

    static size_t &baseIndex()
    {
        return sInstance().mInfo.mBaseIndex;
    }

public:
    template <typename T>
    struct ComponentRegistrator : IndexHolder {    
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

template <typename _Base, typename... _Ty>
UniqueComponentCollector<_Base, _Ty...> &UniqueComponentCollector<_Base, _Ty...>::sInstance()
{
    static UniqueComponentCollector dummy;
    return dummy;
}

}

#else

namespace Engine {

template <typename _Base, typename... _Ty>
struct UniqueComponentCollector {
    typedef _Base Base;
    typedef std::tuple<_Ty...> Ty;
    typedef UniqueComponentRegistry<Base, _Ty...> Registry;
};

}

#endif

//#define RegisterCollector(collector) RegisterType(collector::Registry)
