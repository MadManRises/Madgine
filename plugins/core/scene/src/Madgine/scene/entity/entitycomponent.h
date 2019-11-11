#pragma once

#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

#include "Modules/serialize/streams/serializestream.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DLL_IMPORT_VARIABLE2(const EntityComponentCollector::ComponentRegistrator<T>, _reg, typename T);

        DLL_IMPORT_VARIABLE2(const char *const, _componentName, typename T);

        template <class T, class Base>
        using EntityComponentVirtualImpl = Serialize::SerializableUnit<T, Base>;

        template <class T, class Base = EntityComponentBase>
        class EntityComponent : public Serialize::SerializableUnit<T, Base> {
        public:
			using Serialize::SerializableUnit<T, Base>::SerializableUnit;

            const char *key() const override
            {
                return componentName();
            }

			static const char* componentName() {
                return _componentName<T>();
			}

        private:
            void writeCreationData(Serialize::SerializeOutStream &out) const
            {
                out << componentName();
            }
        };

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, ...) \
    DLL_EXPORT_VARIABLE2(constexpr, const char *const, Engine::Scene::Entity::, _componentName, #Name, __VA_ARGS__)

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    DLL_EXPORT_VARIABLE2(, const Engine::Scene::Entity::EntityComponentCollector::ComponentRegistrator<Name>, Engine::Scene::Entity::, _reg, {}, Name)

#define ENTITYCOMPONENT_IMPL(Name, ...)                                                        \
    DLL_EXPORT_VARIABLE2(constexpr, const char *const, Engine::Scene::Entity::, _componentName, #Name, __VA_ARGS__); \
    DLL_EXPORT_VARIABLE2(, const Engine::Scene::Entity::EntityComponentCollector::ComponentRegistrator<__VA_ARGS__>, Engine::Scene::Entity::, _reg, {}, __VA_ARGS__)

    }
}
}
