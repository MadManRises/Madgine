#pragma once

#include "uniquecomponentregistry.h"

#if ENABLE_PLUGINS

#    include "indexholder.h"

namespace Engine {

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

public:
    template <typename T>
    struct ComponentRegistrator : IndexHolder {
        ComponentRegistrator()
            : IndexHolder{sInstance().mInfo.template registerComponent<T>()
            , sInstance().mInfo.mBaseIndex}
        {
        }

        ~ComponentRegistrator()
        {
            sInstance().mInfo.unregisterComponent(mIndex);
        }
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
