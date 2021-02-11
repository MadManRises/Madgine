#pragma once

namespace Engine {
namespace Audio {

	MADGINE_AUDIOLOADER_EXPORT InStream DecodeOggFile(AudioInfo &info, InStream &&file);

}
}