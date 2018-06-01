#pragma once

#include "uniquecomponentcollector.h"

namespace Engine
{
	template <class Base, class... _Ty>
	class MADGINE_CLIENT_EXPORT OgreCreatorStore
	{
	protected:
		static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents()
		{
			static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>> dummy;
			return dummy;
		}

		static constexpr size_t baseIndex() { return 0; }
	};

	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using OgreUniqueComponentCollector = UniqueComponentCollector<Base, OgreCreatorStore<Base, _Ty...>, Container, _Ty...>;

}
