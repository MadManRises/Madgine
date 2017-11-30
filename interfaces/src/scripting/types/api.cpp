#include "interfaceslib.h"
#include "api.h"

namespace Engine
{
	KeyValueValueFlags kvFlags(const Scripting::Mapper& mapper)
	{
		return mapper.isWritable() ? IsEditable : NoFlags;
	}

	KeyValueValueFlags kvFlags(Scripting::Mapper& mapper)
	{
		return mapper.isWritable() ? IsEditable : NoFlags;
	}

	namespace Scripting
	{
		ValueType toValueType(ScopeBase* ref, const Mapper& mapper)
		{
			return mapper.mGetter(ref);
		}
	}
}
