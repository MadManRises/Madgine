#pragma once


namespace Engine
{
	namespace Serialize
	{

		template <class T>
		class NoParentUnit : public T
		{
		public:
			template <class... Args>
			NoParentUnit(Args&&... args) :
				T(std::forward<Args>(args)...)
			{
				this->postConstruct();
				this->activate();
			}

			NoParentUnit(const NoParentUnit<T>& t) :
				T(t)
			{
				this->postConstruct();
				this->activate();
			}

			NoParentUnit(NoParentUnit<T>&& t) noexcept :
				T(std::forward<NoParentUnit<T>>(t))
			{
				this->postConstruct();
				this->activate();
			}

			~NoParentUnit()
			{
				this->deactivate();
			}
		};
	}
}
