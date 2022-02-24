#include "../../metalib.h"

#include "serializablemapholder.h"

#include "formattedserializestream.h"

#include "serializestreamdata.h"

namespace Engine {
namespace Serialize {

    SerializableMapHolder::SerializableMapHolder(FormattedSerializeStream &out)
        : mData(out.data())
    {
        if (mData){
            if (!mData->mSerializableMap) {
                mData->mSerializableMap = &mMap;
            } else {
                mData = nullptr;
            }
        }
    }

    SerializableMapHolder::~SerializableMapHolder()
    {
        if (mData) {
            assert(mData->mSerializableMap == &mMap);
            mData->mSerializableMap = nullptr;
        }
    }

    SerializableListHolder::SerializableListHolder(FormattedSerializeStream &in)
        : mData(in.data())
    {
        if (mData){
            if (!mData->mSerializableList) {
                mData->mSerializableList = &mList;
            } else {
                mData = nullptr;
            }
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