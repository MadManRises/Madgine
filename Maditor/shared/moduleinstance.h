#pragma once

#include "Madgine/serialize/serializableunit.h"
#include "Madgine/serialize/container/list.h"
#include "Madgine/serialize/container/action.h"
#include "Madgine/serialize/container/observed.h"

namespace Maditor {
	namespace Shared {

		class MADITOR_SHARED_EXPORT ModuleInstance : public Engine::Serialize::SerializableUnit<ModuleInstance> {


		private:
			virtual void reloadImpl();
			void reloadImpl2() {
				reloadImpl();
			}

		public:
			ModuleInstance(const std::string &name);

			const std::string &name() const;

			bool exists() const;
			void setExists(bool b);

			bool isLoaded() const;
			void setLoaded(bool b);

			void addDependency(ModuleInstance *dep);
			void removeDependency(ModuleInstance *dep);

			virtual void writeCreationData(Engine::Serialize::SerializeOutStream &out) const override;

			Engine::Serialize::Action<decltype(&ModuleInstance::reloadImpl2), &ModuleInstance::reloadImpl2, Engine::Serialize::ActionPolicy::request> reload;

			const Engine::Serialize::ObservableList<ModuleInstance*, Engine::Serialize::ContainerPolicies::allowAll> &dependencies();

		protected:
			Engine::Serialize::Observed<bool> mLoaded;


		private:
			bool mExists;
			std::string mName;


			Engine::Serialize::ObservableList<ModuleInstance*, Engine::Serialize::ContainerPolicies::allowAll> mDependencies;

		};


	}
}
