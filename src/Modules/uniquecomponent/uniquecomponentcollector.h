#pragma once

#include "uniquecomponentregistry.h"

#if ENABLE_PLUGINS

#    include "indexholder.h"

namespace Engine {

DERIVE_TYPENAME(VBase)

template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentCollector {
    typedef typename Registry::Base Base;
    typedef std::tuple<_Ty...> Ty;
    typedef typename Registry::F F;

    UniqueComponentCollector()
    {
        mInfo.mRegistryInfo = &typeInfo<Registry>;
        mInfo.mBaseInfo = &typeInfo<Base>;
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
    typename Registry::CollectorInfo mInfo;

    template <typename T>
    static size_t registerComponent()
    {

        LOG("Registering Component: " << typeName<T>());
        sInstance().mInfo.mComponents.emplace_back(&createComponent<T, Base, _Ty...>);
        std::vector<const TypeInfo *> elementInfos;
        elementInfos.push_back(&typeInfo<T>);
        if constexpr (has_typename_VBase_v<T>) {
            elementInfos.push_back(&typeInfo<typename T::VBase>);
        }
        sInstance().mInfo.mElementInfos.emplace_back(std::move(elementInfos));
        return sInstance().mInfo.mComponents.size() - 1;
    }

    static void unregisterComponent(size_t i)
    {
        sInstance().mInfo.mComponents[i] = nullptr;
        sInstance().mInfo.mElementInfos[i].clear();
    }

    static size_t &baseIndex()
    {
        return sInstance().mInfo.mBaseIndex;
    }

public:
    template <typename T>
    struct ComponentRegistrator : IndexHolder {
        ComponentRegistrator()
            : mIndex(registerComponent<T>())
            , mBaseIndex(baseIndex())
        {
        }

        ~ComponentRegistrator()
        {
            unregisterComponent(mIndex);
            mIndex = std::numeric_limits<size_t>::max();
        }

        size_t index() const override
        {
            return mIndex + mBaseIndex;
        }

        bool isValid() const override
        {
            return mIndex != std::numeric_limits<size_t>::max() && mBaseIndex != std::numeric_limits<size_t>::max();
        }

    private:
        size_t mIndex = std::numeric_limits<size_t>::max();
        //Make it a member to prevent problems through weak symbols during link time
        size_t &mBaseIndex;
    };
};

template <typename Registry, typename __Base, typename... _Ty>
UniqueComponentCollector<Registry, __Base, _Ty...> &UniqueComponentCollector<Registry, __Base, _Ty...>::sInstance()
{
    static UniqueComponentCollector dummy;
    return dummy;
}

}

#else

namespace Engine {

template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentCollector {
    typedef typename Registry::Base Base;
    typedef std::tuple<_Ty...> Ty;
    typedef typename Registry::F F;
};

}

#endif

//#define RegisterCollector(collector) RegisterType(collector::Registry)
