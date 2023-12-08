#include "../metalib.h"

#include "visitor.h"

#include "streams/streamresult.h"

#include "Generic/bytebuffer.h"
#include "Meta/math/color4.h"
#include "Meta/math/matrix4.h"

#include "streams/formattedserializestream.h"

namespace Engine {
namespace Serialize {

    template <typename T>
    StreamResult visitSkipPrimitive(FormattedSerializeStream &in, const char *name)
    {
        T dummy;
        return in.readPrimitive<T>(dummy, name);
    }

    StreamResult visitSkipEnum(const EnumMetaTable *table, FormattedSerializeStream &in, const char *name)
    {
        DynamicEnum dummy { table };
        return in.readPrimitive<DynamicEnum>(dummy, name);        
    }

    static void __instantiationHelper(FormattedSerializeStream &in, const char *name)
    {
        TypeUnpacker::forEach<SerializePrimitives>([&]<typename T>() {
            if constexpr (!std::same_as<T, EnumTag>)
                (void)visitSkipPrimitive<T>(in, name);
        });
    }

}
}