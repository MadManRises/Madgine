#include "../metalib.h"

#include "visitor.h"

#include "streams/streamresult.h"

#include "Generic/bytebuffer.h"
#include "../math/color4.h"
#include "../math/matrix4.h"

#include "streams/formattedserializestream.h"

#include "../enumholder.h"
#include "../flagsholder.h"

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
        EnumHolder dummy { table };
        return in.readPrimitive<EnumHolder>(dummy, name);        
    }

    StreamResult visitSkipFlags(const EnumMetaTable *table, FormattedSerializeStream &in, const char *name)
    {
        FlagsHolder dummy { table };
        return in.readPrimitive<FlagsHolder>(dummy, name);        
    }

    void __instantiationHelper(FormattedSerializeStream &in, const char *name)
    {
        TypeUnpacker::forEach<SerializePrimitives>([&]<typename T>() {
            if constexpr (!std::same_as<T, EnumTag> && !std::same_as<T, FlagsTag>)
                (void)visitSkipPrimitive<T>(in, name);
        });
    }

}
}