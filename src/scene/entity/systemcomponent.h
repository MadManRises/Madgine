#pragma once

#include "entitycomponent.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T, class S, class Base = EntityComponentBase>
			class SystemComponent : public EntityComponent<T, Base>
			{
			public:
				using EntityComponent<T, Base>::EntityComponent;

				virtual void init() override
				{
					EntityComponent<T, Base>::init();
					getSceneComponent<S>().addComponent(static_cast<T*>(this));
				}

				virtual void finalize() override
				{
					getSceneComponent<S>().removeComponent(static_cast<T*>(this));
					EntityComponent<T, Base>::finalize();
				}
			};
		}
	}
}
