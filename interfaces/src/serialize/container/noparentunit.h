#pragma once

namespace Engine
{
	namespace Serialize
	{
		template <class T>
		class NoParentUnit : public T
		{
		public:
			template <class... _Ty>
			NoParentUnit(_Ty&&... args) :
				T(std::forward<_Ty>(args)...)
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
