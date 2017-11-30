#pragma once

#include "entitycomponent.h"
#include "system.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T, class Base = EntityComponentBase>
			class SystemComponent : public EntityComponent<T, Base>
			{
			public:
				using EntityComponent<T, Base>::EntityComponent;

				virtual void init() override
				{
					EntityComponent<T, Base>::init();
					System<T>::getSingleton().addComponent(static_cast<T*>(this));
				}

				virtual void finalize() override
				{
					System<T>::getSingleton().removeComponent(static_cast<T*>(this));
					EntityComponent<T, Base>::finalize();
				}
			};
		}
	}
}
