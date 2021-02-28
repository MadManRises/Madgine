#include "../../metalib.h"

#include "serializablemapholder.h"
#include "serializestreambuf.h"

namespace Engine {
namespace Serialize {

    SerializableMapHolder::~SerializableMapHolder()
    {
        if (mData) {
            assert(mData->mSerializableMap == &mMap);
            mData->mSerializableMap = nullptr;
        }
    }

    SerializableListHolder::~SerializableListHolder()
    {
        if (mData) {
            assert(mData->mSerializableList == &mList);
            mData->mSerializableList = nullptr;
        }
    }

}
}