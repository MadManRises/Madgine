#include "interfaceslib.h"

#include "serializehelper.h"

namespace Engine
{
	namespace Serialize
	{
		namespace __tupleserializer__impl__
		{
			void readTuple(std::tuple<>& tuple, SerializeInStream& in, std::index_sequence<>)
			{
			}

			void writeTuple(const std::tuple<>& tuple, SerializeOutStream& out, std::index_sequence<>)
			{
			}
		}
	}
}