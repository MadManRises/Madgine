#pragma once

#include "uniquecomponentcollector.h"

namespace Engine
{
	template <class Base, class... _Ty>
	class OGREMADGINE_EXPORT OgreCreatorStore
	{
	protected:
		static std::list<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents()
		{
			static std::list<std::function<std::unique_ptr<Base>(_Ty ...)>> dummy;
			return dummy;
		}
	};

	template <class Base, template <class...> class Container = std::list, class... _Ty>
	class OGREMADGINE_EXPORT OgreUniqueComponentCollector :
		public UniqueComponentCollector<Base, OgreCreatorStore<Base, _Ty...>, Container, _Ty...>,
		public Singleton<OgreUniqueComponentCollector<Base>>
	{
		using UniqueComponentCollector<Base, OgreCreatorStore<Base, _Ty...>, Container, _Ty...>::UniqueComponentCollector;
	};
}
