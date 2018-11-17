#pragma once

namespace Engine
{

	template <class T>
	class TupleConstructed : public T
	{
	public:
		template <class... Ty>
		TupleConstructed(const std::tuple<Ty...>& tuple) :
			TupleConstructed(std::make_index_sequence<sizeof...(Ty)>(), tuple)
		{
		}

	private:
		template <class Tuple, size_t... Is>
		TupleConstructed(std::index_sequence<Is...>, const Tuple& tuple) :
			T(std::get<Is>(tuple)...)
		{
		}
	};

}