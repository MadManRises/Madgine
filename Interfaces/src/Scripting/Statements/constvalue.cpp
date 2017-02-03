#include "interfaceslib.h"
#include "constvalue.h"

namespace Engine {
namespace Scripting {
namespace Statements {


ConstValue::ConstValue(int line, const ValueType &v) :
    mValue(v),
	Statement(line)
{

}

ValueType ConstValue::run(Scope *, Scope *, Stack &, bool *) const
{
    return mValue;
}

}
}
}
