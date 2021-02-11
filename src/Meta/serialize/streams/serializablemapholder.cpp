#include "../../metalib.h"

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

    SerializableListHolder::~SerializableListHolder()
    {
        if (mBuffer) {
            assert(mBuffer->mSerializableList == &mList);
            mBuffer->mSerializableList = nullptr;
        }
    }

}
}