#pragma once

#include "Modules/plugins/pluginlocal.h"

#include "Modules/keyvalue/objectptr.h"

#include "Modules/keyvalue/metatable.h"

namespace Engine {
namespace Scene {
    namespace Entity {

		struct PluginEntityComponentInfo {
            std::function<std::unique_ptr<EntityComponentBase>(Entity &, const ObjectPtr &)> mCtor;
            const MetaTable *mType;
		};

        struct PluginEntityComponents {
            std::map<std::string, PluginEntityComponentInfo> mComponents;
        };

#if ENABLE_PLUGINS
        extern "C" DLL_EXPORT PluginEntityComponents *PLUGIN_LOCAL(pluginEntityComponents)();
#endif

        struct MADGINE_SCENE_EXPORT EntityComponentCollector {        
            static bool existsComponent(const std::string &name);

            static std::set<std::string> registeredComponentNames();

            static std::unique_ptr<EntityComponentBase> createComponent(Entity &e, const std::string &name,
                const ObjectPtr &table = {});

			static const MetaTable *getComponentType(const std::string &name);

        private:
            typedef std::function<std::unique_ptr<EntityComponentBase>(Entity &, const ObjectPtr &)> ComponentBuilder;
#if ENABLE_PLUGINS
            struct LocalComponentStore {
                static PluginEntityComponents *sRegisteredComponentsByName()
                {
                    static PluginEntityComponents dummy;
                    return &dummy;
                }
            };

            friend PluginEntityComponents *PLUGIN_LOCAL(pluginEntityComponents)()
            {
                return LocalComponentStore::sRegisteredComponentsByName();
            }

#define PLUGABLE_COMPONENT LocalComponentStore::

#else

#define PLUGABLE_COMPONENT

#endif

            template <typename T>
            static std::unique_ptr<EntityComponentBase> createComponent_t(Entity &e, const ObjectPtr &table = {})
            {
                return std::make_unique<T>(e, table);
            }

        public:
            template <typename T>
            struct ComponentRegistrator {            
                ComponentRegistrator()
                {
                    const std::string name = T::componentName();
                    assert(PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents.find(name) == PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents.end());
                    PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents[name] = { &createComponent_t<T>, &table<T>() };
                }

                ~ComponentRegistrator()
                {
                    const std::string name = T::componentName();
                    assert(PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents.find(name) != PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents.end());
                    PLUGABLE_COMPONENT sRegisteredComponentsByName()->mComponents.erase(name);
                }
            };

        private:
            static PluginEntityComponents *sRegisteredComponentsByName();
        };

    }
}
}