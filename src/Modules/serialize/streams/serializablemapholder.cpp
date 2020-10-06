#include "../../moduleslib.h"

#include "serializablemapholder.h"
#include "serializestreambuf.h"

namespace Engine {
namespace Serialize {

    SerializableMapHolder::~SerializableMapHolder()
    {
        if (mBuffer) {
            assert(mBuffer->mSerializableMap == &mMap);
            mBuffer->mSerializableMap = nullptr;
        }
    }

}
}