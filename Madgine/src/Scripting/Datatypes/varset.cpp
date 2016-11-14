#include "libinclude.h"
#include "varset.h"
#include "Scripting/scriptingexception.h"
#include <fstream>
#include "Serialize/serializestream.h"

#include "Database/exceptionmessages.h"

#include "Util\UtilMethods.h"

namespace Engine {
namespace Scripting {

const ValueType &VarSet::get(const std::string &name) const
{
    if (!contains(name)) 
		MADGINE_THROW_NO_TRACE(ScriptingException(Database::Exceptions::unknownVariable(name)));
    return at(name);
}

bool VarSet::contains(const std::string &name) const
{
    return find(name) != end();
}

void VarSet::save(Serialize::SerializeOutStream &out) const
{
    for (const std::pair<std::string, ValueType> &p : *this) {
        out << p.first << p.second;
    }

    out << ValueType();
}

void VarSet::load(Serialize::SerializeInStream &in)
{
	std::string key;

    while (in.loopRead(key)) {
        in >> (*this)[key];
    }
}


}
}

