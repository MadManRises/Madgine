#include "../../baselib.h"

#include "entitycomponentcollector.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "componentexception.h"

#include "entity.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			

			bool EntityComponentCollector::existsComponent(const std::string& name)
			{
				return sRegisteredComponentsByName()->mComponents.find(name) != sRegisteredComponentsByName()->mComponents.end();
			}

			std::set<std::string> EntityComponentCollector::registeredComponentNames()
			{
				std::set<std::string> result;

				for (const std::pair<const std::string, ComponentBuilder>& p : sRegisteredComponentsByName()->mComponents)
				{
					result.insert(p.first);
				}

				return result;
			}

			std::unique_ptr<EntityComponentBase> EntityComponentCollector::createComponent(Entity &e,
				const std::string& name, const Scripting::LuaTable& table)
			{
				auto it = sRegisteredComponentsByName()->mComponents.find(name);
				if (it == sRegisteredComponentsByName()->mComponents.end()) {
#ifndef STATIC_BUILD
					typedef PluginEntityComponents *(*ComponentGetter)();
					for (Plugins::PluginSection &section : kvValues(Plugins::PluginManager::getSingleton())) {
						for (const Plugins::Plugin &plugin : kvValues(section)) {
							ComponentGetter getter = (ComponentGetter)plugin.getSymbol("pluginEntityComponents");
							if (getter) {
								it = getter()->mComponents.find(name);
								if (it != getter()->mComponents.end()) {
									return it->second(e, table);
								}
							}
						}
					}
#endif
					throw ComponentException(Database::Exceptions::unknownComponent(name));
				}
				return it->second(e, table);
			}

			PluginEntityComponents* EntityComponentCollector::sRegisteredComponentsByName()
			{
				static PluginEntityComponents dummy;
				return &dummy;
			}


		}
	}
}