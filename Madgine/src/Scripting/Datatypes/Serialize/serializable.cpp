#include "libinclude.h"
#include "serializable.h"

#include "serializestream.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

SerializeInStream operator >>(SerializeInStream &in, Serializable &s)
{
    s.load(in);
    return in;
}

SerializeOutStream operator <<(SerializeOutStream &out, const Serializable &s)
{
    s.save(out);
    return out;
}



} // namespace Serialize
} // namespace Scripting
} // namespace Core

