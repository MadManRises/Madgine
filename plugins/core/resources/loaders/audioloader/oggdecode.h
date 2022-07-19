#pragma once

namespace Engine {
namespace Audio {

	MADGINE_AUDIOLOADER_EXPORT Stream DecodeOggFile(AudioInfo &info, Stream &&file);

}
}