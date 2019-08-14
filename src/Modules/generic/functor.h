#pragma once

namespace Engine
{
	
	template <auto F>
	struct Functor {
		template <typename... Ty>
		decltype(auto) operator()(Ty&&... args)
		{
			return F(std::forward<Ty>(args)...);
		}
	};

}