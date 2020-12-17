#include "audioloaderlib.h"

#include "oggdecode.h"

#include "oggdecodebuf.h"

#include "Interfaces/streams/streams.h"

namespace Engine {
namespace Audio {

    InStream DecodeOggFile(AudioInfo &info, InStream &&file)
    {
        return { std::make_unique<OggDecodeBuf>(info, file.release()) };
    }

}
}