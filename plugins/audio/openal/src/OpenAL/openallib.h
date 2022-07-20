#pragma once

/// @cond

#include "Madgine/audioloaderlib.h"
#include "Madgine/scenelib.h"

#if defined(OpenALPlugin_EXPORTS)
#    define MADGINE_OPENAL_EXPORT DLL_EXPORT
#else
#    define MADGINE_OPENAL_EXPORT DLL_IMPORT
#endif

#include "openalforward.h"

#include <AL/al.h>
#include <AL/alc.h>

inline void al_check()
{
    ALCenum e = alGetError();
    if (e != AL_NO_ERROR) {
        {
            Engine::Util::LogDummy out { Engine::Util::MessageType::ERROR_TYPE };
            out << "GL-Error: ";
            switch (e) {
                CONSTANT_CASE(AL_INVALID_NAME, out)
                CONSTANT_CASE(AL_INVALID_ENUM, out)
                CONSTANT_CASE(AL_INVALID_VALUE, out)
                CONSTANT_CASE(AL_INVALID_OPERATION, out)
                CONSTANT_CASE(AL_OUT_OF_MEMORY, out)
            default:
                out << "UNKNOWN";
            }
            out << "(" << e << ")";
        }
        std::terminate();
    }
}

#define AL_CHECK() al_check();


/// @endcond
