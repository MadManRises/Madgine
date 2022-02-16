#include "audioloaderlib.h"

#include "oggdecode.h"

#include "oggdecodebuf.h"

#include "Generic/stream.h"

namespace Engine {
namespace Audio {

    Stream DecodeOggFile(AudioInfo &info, Stream &&file)
    {
        return { std::make_unique<OggDecodeBuf>(info, file.release()) };
    }

}
}