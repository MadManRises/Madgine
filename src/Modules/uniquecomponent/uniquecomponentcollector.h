#pragma once

#include "uniquecomponentregistry.h"

#if ENABLE_PLUGINS

#    include "indexholder.h"

namespace Engine {
namespace UniqueComponent {

    template <typename Registry, typename __Base, typename... _Ty>
    struct Collector {
        typedef typename Registry::Base Base;
        typedef std::tuple<_Ty...> Ty;
        typedef typename Registry::F F;

        Collector()
        {
            mInfo.mRegistryInfo = &typeInfo<Registry>;
            mInfo.mBaseInfo = &typeInfo<Base>;
            mInfo.mBinary = &Plugins::PLUGIN_LOCAL(binaryInfo);
            Registry::sInstance().addCollector(&mInfo);
        }
        Collector(const Collector &) = delete;
        ~Collector()
        {
            Registry::sInstance().removeCollector(&mInfo);
        }

        void operator=(const Collector &) = delete;

        static Collector &sInstance();

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
                : IndexHolder { sInstance().mInfo.template registerComponent<T>(), sInstance().mInfo.mBaseIndex }
            {
            }

            ~ComponentRegistrator()
            {
                sInstance().mInfo.unregisterComponent(mIndex);
            }
        };
    };

    template <typename Registry, typename __Base, typename... _Ty>
    Collector<Registry, __Base, _Ty...> &Collector<Registry, __Base, _Ty...>::sInstance()
    {
        static Collector dummy;
        return dummy;
    }

}
}

#else

namespace Engine {
namespace UniqueComponent {

    template <typename Registry, typename __Base, typename... _Ty>
    struct Collector {
        typedef typename Registry::Base Base;
        typedef std::tuple<_Ty...> Ty;
        typedef typename Registry::F F;
    };

}
}

#endif
