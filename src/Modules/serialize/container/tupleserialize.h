#pragma once

namespace Engine
{
	namespace Serialize
	{
		
	namespace __tupleserializer__impl__{
		

		template <class Arg, class... Args, size_t... S>
		void readTuple(std::tuple<Arg, Args...>& tuple, SerializeInStream& in, std::index_sequence<S...>)
		{
			(in >> ... >> std::get<S>(tuple));
		}

		inline void readTuple(std::tuple<>& tuple, SerializeInStream& in, std::index_sequence<>){}
                
		template <class Arg, class... Args, size_t... S>
		static void writeTuple(const std::tuple<Arg, Args...>& tuple, SerializeOutStream& out,
		                       std::index_sequence<S...>)
		{
			(out << ... << std::get<S>(tuple));
		}

		inline void writeTuple(const std::tuple<>& tuple, SerializeOutStream& out, std::index_sequence<>){}

	}

	template <class... Args>
	static SerializeInStream &operator >>(SerializeInStream& in, std::tuple<Args...>& tuple)
	{
		__tupleserializer__impl__::readTuple(tuple, in, std::make_index_sequence<sizeof...(Args)>());
		return in;
	}



	template <class... Args>
	static SerializeOutStream &operator <<(SerializeOutStream& out, const std::tuple<Args...>& tuple)
	{
		__tupleserializer__impl__::writeTuple(tuple, out, std::make_index_sequence<sizeof...(Args)>());
		return out;
	}		


	}
}