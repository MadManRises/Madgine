#include "interfaceslib.h"
#include "argumentlist.h"
#include "serialize/streams/serializestream.h"
#include "scripting/types/globalscopebase.h"

#include "scripting/types/api.h"

namespace Engine{
namespace Scripting{

	ArgumentList::ArgumentList(lua_State * state, int count)
	{
		resize(count);
		for (int i = 0; i < count; ++i) {
			at(i) = APIHelper::to<ValueType>(state, i - count);
		}
		APIHelper::pop(state, count);
	}

	void ArgumentList::readState(Serialize::SerializeInStream &in)
	{
		clear();
		ValueType temp;
		while (in.loopRead(temp)) {
			emplace_back(std::move(temp));
		}
	}

	void ArgumentList::writeState(Serialize::SerializeOutStream &out) const
	{
		for (const ValueType &v : *this) {
			out << v;
		}
		out << ValueType::EOL();
	}

	void ArgumentList::pushToStack(lua_State *state) const {
		for (const ValueType &v : *this) {
			APIHelper::push(state, v);
		}
	}


}
}
