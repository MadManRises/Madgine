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

    template StreamResult visitSkipPrimitive<bool>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<uint8_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<int8_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<uint16_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<int16_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<uint32_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<int32_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<uint64_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<int64_t>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<float>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<SyncableUnitBase*>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<SerializableDataUnit *>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<std::string>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<ByteBuffer>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Void>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Vector2>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Vector3>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Vector4>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Matrix3>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Color3>(FormattedSerializeStream &, const char *);
    template StreamResult visitSkipPrimitive<Color4>(FormattedSerializeStream &, const char *);

}
}