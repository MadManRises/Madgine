#pragma once

#include "../plugins/binaryinfo.h"

namespace Engine {
namespace UniqueComponent {

    DERIVE_TYPENAME(VBase)

    template <typename T, typename Base, typename... _Ty>
    std::unique_ptr<Base> createComponent(_Ty... arg)
    {
        return std::make_unique<T>(std::forward<_Ty>(arg)...);
    }

    template <typename Base, typename... _Ty>
    using Collector_F = std::unique_ptr<Base> (*)(_Ty...);

}
}

#if ENABLE_PLUGINS

namespace Engine {
namespace UniqueComponent {

    struct CollectorInfoBase {
        const TypeInfo *mRegistryInfo;
        const TypeInfo *mBaseInfo;
        const Plugins::BinaryInfo *mBinary;
        std::vector<std::vector<const TypeInfo *>> mElementInfos;
        IndexType<size_t> mBaseIndex;
        std::vector<std::string_view> mComponentNames;
    };

    MODULES_EXPORT std::vector<RegistryBase *> &registryRegistry();

    struct MODULES_EXPORT RegistryBase {
        RegistryBase(const TypeInfo *ti, const Plugins::BinaryInfo *binary, const TypeInfo *namedTi = nullptr)
            : mBinary(binary)
            , mTi(ti)
            , mNamedTi(namedTi ? namedTi : ti)
        {
            LOG("Adding: " << ti->mTypeName);
            registryRegistry().push_back(this);
        }

        ~RegistryBase()
        {
            std::erase(registryRegistry(), this);
        }

        virtual void onPluginLoad(const Plugins::BinaryInfo *) = 0;
        virtual void onPluginUnload(const Plugins::BinaryInfo *) = 0;

        const TypeInfo *type_info()
        {
            return mTi;
        }

        const TypeInfo *named_type_info()
        {
            return mNamedTi;
        }

        std::vector<CollectorInfoBase *>::iterator begin()
        {
            return mLoadedCollectors.begin();
        }

        std::vector<CollectorInfoBase *>::iterator end()
        {
            return mLoadedCollectors.end();
        }

        const Plugins::BinaryInfo *mBinary;

        std::map<std::string_view, IndexType<uint32_t>> mComponentsByName;
        bool mIsNamed = false;

    protected:
        std::vector<CollectorInfoBase *> mLoadedCollectors;

    private:
        const TypeInfo *mTi;
        const TypeInfo *mNamedTi;
    };

    DLL_IMPORT_VARIABLE2(Registry, registry, typename Registry);

    template <typename _Base, typename... _Ty>
    struct Registry : RegistryBase {

        typedef _Base Base;
        typedef std::tuple<_Ty...> Ty;
        typedef Collector_F<Base, _Ty...> F;

        struct CollectorInfo : CollectorInfoBase {

            template <typename T>
            size_t registerComponent()
            {

                LOG("Registering Component: " << typeName<T>());
                mComponents.emplace_back(&createComponent<T, Base, _Ty...>);
                std::vector<const TypeInfo *> elementInfos;
                elementInfos.push_back(&typeInfo<T>);
                if constexpr (has_typename_VBase<T>) {
                    elementInfos.push_back(&typeInfo<typename T::VBase>);
                }
                mElementInfos.emplace_back(std::move(elementInfos));
                return mComponents.size() - 1;
            }

            void unregisterComponent(size_t i)
            {
                mComponents[i] = nullptr;
                mElementInfos[i].clear();
            }

            std::vector<F> mComponents;
        };

        Registry(const TypeInfo *namedTi = nullptr)
            : RegistryBase(&typeInfo<Registry>, &Plugins::PLUGIN_LOCAL(binaryInfo), namedTi)
        {
        }

        static Registry &sInstance()
        {
            return registry<Registry<_Base, _Ty...>>();
        }

        static std::vector<F> &sComponents()
        {
            return sInstance().mComponents;
        }

        static F getConstructor(size_t i)
        {
            return sInstance().mComponents[i];
        }

        void addCollector(CollectorInfo *info)
        {
            mUnloadedCollectors.push_back(info);
        }

        void onPluginLoad(const Plugins::BinaryInfo *bin)
        {
            for (auto it = mUnloadedCollectors.begin(); it != mUnloadedCollectors.end();) {
                CollectorInfo *info = *it;
                if (info->mBinary == bin) {
                    mLoadedCollectors.push_back(info);
                    info->mBaseIndex = mComponents.size();
                    const std::vector<F> &comps = reinterpret_cast<const std::vector<F> &>(info->mComponents);
                    for (F f : comps) {
                        mComponents.push_back(f);
                    }
                    it = mUnloadedCollectors.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void removeCollector(CollectorInfoBase *info)
        {
            //assert(std::find(mLoadedCollectors.begin(), mLoadedCollectors.end(), info) == mLoadedCollectors.end());
            std::erase(mUnloadedCollectors, info);
        }

        void onPluginUnload(const Plugins::BinaryInfo *bin)
        {
            for (auto it = mLoadedCollectors.begin(); it != mLoadedCollectors.end();) {
                CollectorInfo *info = static_cast<CollectorInfo *>(*it);
                if (info->mBinary == bin) {
                    mUnloadedCollectors.push_back(info);
                    it = mLoadedCollectors.erase(it);
                    mComponents.erase(mComponents.begin() + info->mBaseIndex, mComponents.begin() + info->mBaseIndex + info->mComponents.size());

                    for (CollectorInfoBase *i : mLoadedCollectors) {
                        if (i->mBaseIndex >= info->mBaseIndex)
                            i->mBaseIndex -= info->mComponents.size();
                    }

                    info->mBaseIndex.reset();
                } else {
                    ++it;
                }
            }
        }

    protected:
        static inline Registry *sSelf = &sInstance(); //Keep to ensure instantiation of registry, even with no component/collector in it

        std::vector<F> mComponents;

        std::vector<CollectorInfo *> mUnloadedCollectors;
    };

    template <typename _Base, typename... _Ty>
    struct NamedRegistry : Registry<_Base, _Ty...> {

        struct CollectorInfo : Registry<_Base, _Ty...>::CollectorInfo {
            template <typename T>
            size_t registerComponent()
            {

                this->mComponentNames.emplace_back(T::componentName());
                return Registry<_Base, _Ty...>::CollectorInfo::template registerComponent<T>();
            }

            void unregisterComponent(size_t i)
            {
                Registry<_Base, _Ty...>::CollectorInfo::unregisterComponent(i);
                this->mComponentNames[i] = {};
            }
        };

        NamedRegistry()
            : Registry<_Base, _Ty...>(&typeInfo<NamedRegistry>)
        {
            this->mIsNamed = true;
        }

        static NamedRegistry &sInstance()
        {
            return static_cast<NamedRegistry &>(registry<Registry<_Base, _Ty...>>());
        }

        static const std::map<std::string_view, IndexType<uint32_t>> &sComponentsByName()
        {
            return sInstance().mComponentsByName;
        }

        void onPluginLoad(const Plugins::BinaryInfo *bin, CompoundAtomicOperation &op)
        {
            size_t counter = this->mComponentsByName.size();

            for (typename Registry<_Base, _Ty...>::CollectorInfo *info : this->mUnloadedCollectors) {
                if (info->mBinary == bin) {
                    const std::vector<std::string_view> &names = static_cast<CollectorInfo *>(info)->mComponentNames;
                    for (std::string_view name : names) {
                        this->mComponentsByName[name] = counter;
                        ++counter;
                    }
                }
            }

            Registry<_Base, _Ty...>::onPluginLoad(bin, op);
        }
    };

}
}

#else

namespace Engine {
namespace UniqueComponent {

    template <typename _Base, typename... _Ty>
    struct Registry {

        typedef _Base Base;
        typedef std::tuple<_Ty...> Ty;
        typedef Collector_F<Base, _Ty...> F;

        static std::vector<F> sComponents();

        static F getConstructor(size_t i)
        {
            return sComponents()[i];
        }
    };

    template <typename _Base, typename... _Ty>
    struct NamedRegistry : Registry<_Base, _Ty...> {

        static std::map<std::string_view, size_t> sComponentsByName();
    };

}
}
#endif