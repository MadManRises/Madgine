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

public:
    template <typename T>
    struct ComponentRegistrator : IndexHolder {
        ComponentRegistrator()
            : mIndex(sInstance().mInfo.template registerComponent<T>())
            , mBaseIndex(sInstance().mInfo.mBaseIndex)
        {
        }

        ~ComponentRegistrator()
        {
            sInstance().mInfo.unregisterComponent(mIndex);
            mIndex.reset();
        }

        size_t index() const override
        {
            assert(isValid());
            return mIndex + mBaseIndex;
        }

        bool isValid() const override
        {
            return mIndex && mBaseIndex;
        }

    private:
        IndexType<size_t> mIndex;
        //Make it a member to prevent problems through weak symbols during link time
        IndexType<size_t> &mBaseIndex;
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
