#include "interfaceslib.h"

#include "comparestreamid.h"
#include "serializestream.h"

namespace Engine
{
	namespace Serialize
	{
		
		bool CompareStreamId::operator()(Stream* first, Stream* second) const
		{
			return first->id() < second->id();
		}

	}
}