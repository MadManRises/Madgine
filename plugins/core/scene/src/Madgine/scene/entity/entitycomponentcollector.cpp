#include "../../scenelib.h"

#include "entitycomponentcollector.h"

#include "Modules/plugins/pluginmanager.h"

#include "componentexception.h"

#include "Modules/keyvalue/keyvalue.h"

#include "entitycomponentbase.h"

#include "Modules/plugins/pluginsection.h"

#include "Modules/plugins/plugin.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        bool EntityComponentCollector::existsComponent(const std::string &name)
        {
            return sRegisteredComponentsByName()->mComponents.find(name) != sRegisteredComponentsByName()->mComponents.end();
        }

        std::set<std::string> EntityComponentCollector::registeredComponentNames()
        {
            std::set<std::string> result;

            for (const std::pair<const std::string, PluginEntityComponentInfo> &p : sRegisteredComponentsByName()->mComponents) {
                result.insert(p.first);
            }

#if ENABLE_PLUGINS
            typedef PluginEntityComponents *(*ComponentGetter)();
            for (Plugins::PluginSection &section : kvValues(Plugins::PluginManager::getSingleton())) {
                for (const Plugins::Plugin &plugin : kvValues(section)) {
                    if (plugin.isLoaded()) {
                        ComponentGetter getter = (ComponentGetter)plugin.getSymbol("pluginEntityComponents");
                        if (getter) {
                            for (const std::string &name : kvKeys(getter()->mComponents)) {
                                result.insert(name);
                            }
                        }
                    }
                }
            }
#endif

            return result;
        }

        std::unique_ptr<EntityComponentBase> EntityComponentCollector::createComponent(Entity &e,
            const std::string &name, const ObjectPtr &table)
        {
            auto it = sRegisteredComponentsByName()->mComponents.find(name);
            if (it == sRegisteredComponentsByName()->mComponents.end()) {
#if ENABLE_PLUGINS
                typedef PluginEntityComponents *(*ComponentGetter)();
                for (Plugins::PluginSection &section : kvValues(Plugins::PluginManager::getSingleton())) {
                    for (const Plugins::Plugin &plugin : kvValues(section)) {
                        if (plugin.isLoaded()) {
                            ComponentGetter getter = (ComponentGetter)plugin.getSymbol("pluginEntityComponents");
                            if (getter) {
                                it = getter()->mComponents.find(name);
                                if (it != getter()->mComponents.end()) {
                                    return it->second.mCtor(e, table);
                                }
                            }
                        }
                    }
                }
#endif
                throw ComponentException(Database::Exceptions::unknownComponent(name));
            }
            return it->second.mCtor(e, table);
        }

        const MetaTable *EntityComponentCollector::getComponentType(const std::string &name)
        {
            auto it = sRegisteredComponentsByName()->mComponents.find(name);
            if (it == sRegisteredComponentsByName()->mComponents.end()) {
#if ENABLE_PLUGINS
                typedef PluginEntityComponents *(*ComponentGetter)();
                for (Plugins::PluginSection &section : kvValues(Plugins::PluginManager::getSingleton())) {
                    for (const Plugins::Plugin &plugin : kvValues(section)) {
                        if (plugin.isLoaded()) {
                            ComponentGetter getter = (ComponentGetter)plugin.getSymbol("pluginEntityComponents");
                            if (getter) {
                                it = getter()->mComponents.find(name);
                                if (it != getter()->mComponents.end()) {
                                    return it->second.mType;
                                }
                            }
                        }
                    }
                }
#endif
                throw ComponentException(Database::Exceptions::unknownComponent(name));
            }
            return it->second.mType;
        }

        PluginEntityComponents *EntityComponentCollector::sRegisteredComponentsByName()
        {
            static PluginEntityComponents dummy;
            return &dummy;
        }

    }
}
}