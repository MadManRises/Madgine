#pragma once

#include "entitycomponent.h"
#include "system.h"

namespace Engine {
	namespace Scene {
		namespace Entity {

			template <class T, class Base = EntityComponentBase>
			class SystemComponent : public EntityComponent<T, Base>{
			public:
				using EntityComponent::EntityComponent;

				virtual void init() override {
					EntityComponent::init();
					System<T>::getSingleton().addComponent(static_cast<T*>(this));
				}

				virtual void finalize() override {
					System<T>::getSingleton().removeComponent(static_cast<T*>(this));
					EntityComponent::finalize();
				}
			};

		}
	}
}