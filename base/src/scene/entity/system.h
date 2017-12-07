#pragma once

#include "scene/scenecomponent.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T, class C>
			class System : public SceneComponent<T>
			{
			public:
				using SceneComponent<T>::SerializableUnit;
				virtual ~System() = default;

				virtual void addComponent(C* c)
				{
					mComponents.push_back(c);
				}

				virtual void removeComponent(C* c)
				{
					mComponents.remove(c);
				}

				const std::list<C*>& components()
				{
					return mComponents;
				}

			private:
				std::list<C*> mComponents;
			};
		}
	}
}
