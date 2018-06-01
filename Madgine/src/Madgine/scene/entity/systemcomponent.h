#pragma once

#include "entitycomponent.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T, class S, class Base = EntityComponent<T, EntityComponentBase>>
			class SystemComponent : public Base
			{
			public:
				using Base::Base;

				virtual void init() override
				{
					Base::init();
					getSceneComponent<S>().addComponent(static_cast<T*>(this));
				}

				virtual void finalize() override
				{
					getSceneComponent<S>().removeComponent(static_cast<T*>(this));
					Base::finalize();
				}
			};
		}
	}
}
