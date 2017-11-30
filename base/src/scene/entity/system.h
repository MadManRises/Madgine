#pragma once

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T>
			class System : public Singleton<System<T>>
			{
			public:
				virtual ~System() = default;

				virtual void addComponent(T* t)
				{
					mComponents.push_back(t);
				}

				virtual void removeComponent(T* t)
				{
					mComponents.remove(t);
				}

				const std::list<T*>& components()
				{
					return mComponents;
				}

			private:
				std::list<T*> mComponents;
			};
		}
	}
}
