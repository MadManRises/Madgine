#pragma once

#include "Interfaces/scripting/datatypes/luatable.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			

#ifdef PLUGIN_BUILD
			extern "C" DLL_EXPORT std::map<std::string, std::function<std::unique_ptr<EntityComponentBase>(Entity&, const Scripting::LuaTable&)>> &pluginEntityComponents();
#endif

			class MADGINE_BASE_EXPORT EntityComponentCollector
			{
			public:
				static bool existsComponent(const std::string& name);

				static std::set<std::string> registeredComponentNames();

				static std::unique_ptr<EntityComponentBase> createComponent(Entity &e, const std::string& name,
					const Scripting::LuaTable& table = {});

			private:
				typedef std::function<std::unique_ptr<EntityComponentBase>(Entity&, const Scripting::LuaTable&)> ComponentBuilder;
#ifdef PLUGIN_BUILD
				struct LocalComponentStore {
					static std::map<std::string, ComponentBuilder> &sRegisteredComponentsByName() {
						static std::map<std::string, ComponentBuilder> dummy;
						return dummy;
					}
				};

				friend std::map<std::string, ComponentBuilder> &pluginEntityComponents() {
					return LocalComponentStore::sRegisteredComponentsByName();
				}

#define PLUGABLE_COMPONENT LocalComponentStore::

#else

#define PLUGABLE_COMPONENT 

#endif

				template <class T>
				static std::unique_ptr<EntityComponentBase> createComponent_t(Entity& e, const Scripting::LuaTable &table = {})
				{
					return std::make_unique<T>(e, table);
				}

			public:
				template <class T>
				class ComponentRegistrator
				{
				public:
					ComponentRegistrator()
					{
						const std::string name = T::componentName();
						assert(PLUGABLE_COMPONENT sRegisteredComponentsByName().find(name) == PLUGABLE_COMPONENT sRegisteredComponentsByName().end());
						PLUGABLE_COMPONENT sRegisteredComponentsByName()[name] = &createComponent_t<T>;
					}

					~ComponentRegistrator()
					{
						const std::string name = T::componentName();
						assert(PLUGABLE_COMPONENT sRegisteredComponentsByName().find(name) != PLUGABLE_COMPONENT sRegisteredComponentsByName().end());
						PLUGABLE_COMPONENT sRegisteredComponentsByName().erase(name);
					}
				};

			private:
				static std::map<std::string, ComponentBuilder>& sRegisteredComponentsByName();

			};

		}
	}
}