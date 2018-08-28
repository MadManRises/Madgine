#include "../../baselib.h"

#include "entitycomponentcollector.h"

#include "../../plugins/pluginmanager.h"

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
				return sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end();
			}

			std::set<std::string> EntityComponentCollector::registeredComponentNames()
			{
				std::set<std::string> result;

				for (const std::pair<const std::string, ComponentBuilder>& p : sRegisteredComponentsByName())
				{
					result.insert(p.first);
				}

				return result;
			}

			std::unique_ptr<EntityComponentBase> EntityComponentCollector::createComponent(Entity &e,
				const std::string& name, const Scripting::LuaTable& table)
			{
				auto it = sRegisteredComponentsByName().find(name);
				if (it == sRegisteredComponentsByName().end()) {
					typedef std::map<std::string, ComponentBuilder> & (*ComponentGetter)();
					for (const std::pair<const std::string, Plugins::PluginSection> &sec : Plugins::PluginManager::getSingleton()) {
						for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
							ComponentGetter getter = (ComponentGetter)p.second.getSymbol("pluginEntityComponents");
							if (getter) {
								it = (*getter)().find(name);
								if (it != (*getter)().end()) {
									return it->second(e, table);
								}
							}
						}
					}
					throw ComponentException(Database::Exceptions::unknownComponent(name));
				}
				return it->second(e, table);
			}

			std::map<std::string, EntityComponentCollector::ComponentBuilder>& EntityComponentCollector::sRegisteredComponentsByName()
			{
				static std::map<std::string, ComponentBuilder> dummy;
				return dummy;
			}


		}
	}
}