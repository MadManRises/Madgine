#include "../../moduleslib.h"
#include "argumentlist.h"
#include "../../keyvalue/valuetype.h"
#include "../types/apihelper.h"

namespace Engine {
namespace Scripting {

    ArgumentList popFromStack(lua_State *state, int count)
    {
        ArgumentList result;
        result.resize(count);
        for (int i = 0; i < count; ++i) {
            result.at(i) = ValueType::fromStack(state, i - count);
        }
        APIHelper::pop(state, count);
        return result;
    }
	
	void pushToStack(lua_State *state, const ArgumentList &args) 
    {
        for (const ValueType &v : args) {
            v.push(state);
        }
    }


}
}
