#pragma once

namespace Engine
{
	namespace Serialize
	{
		
	namespace __tupleserializer__impl__{
		

		template <class Arg, class... Args, size_t... S>
		void readTuple(std::tuple<Arg, Args...>& tuple, SerializeInStream& in, std::index_sequence<S...>)
		{
			using expander = int[];
			(void)expander{
				(void(in >> std::get<S>(tuple)), 0)...
			};
		}

		void INTERFACES_EXPORT readTuple(std::tuple<>& tuple, SerializeInStream& in, std::index_sequence<>);

		template <class Arg, class... Args, size_t... S>
		static void writeTuple(const std::tuple<Arg, Args...>& tuple, SerializeOutStream& out,
		                       std::index_sequence<S...>)
		{
			using expander = int[];
			(void)expander{
				(void(out << std::get<S>(tuple)), 0)...
			};
		}

		void INTERFACES_EXPORT writeTuple(const std::tuple<>& tuple, SerializeOutStream& out, std::index_sequence<>);

	}


	struct TupleSerializer
	{
		template <class... Args>
		static void readTuple(std::tuple<Args...>& tuple, SerializeInStream& in)
		{
			__tupleserializer__impl__::readTuple(tuple, in, std::make_index_sequence<sizeof...(Args)>());
		}



		template <class... Args>
		static void writeTuple(const std::tuple<Args...>& tuple, SerializeOutStream& out)
		{
			__tupleserializer__impl__::writeTuple(tuple, out, std::make_index_sequence<sizeof...(Args)>());
		}		
		
	};


	}
}