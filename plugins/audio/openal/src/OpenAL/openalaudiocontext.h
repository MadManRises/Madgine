#pragma once

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"

#include "Generic/functor.h"

struct ALCdevice;
struct ALCcontext;

namespace Engine {
namespace Audio {

    struct MADGINE_OPENAL_EXPORT OpenALAudioContext : public App::GlobalAPI<OpenALAudioContext> {
        OpenALAudioContext(App::Application &app);
        ~OpenALAudioContext();

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

    private:
        std::unique_ptr<ALCdevice, Functor<&alcCloseDevice>> mDevice;
        std::unique_ptr<ALCcontext, Functor<&alcDestroyContext>> mContext;
    };

}
}

REGISTER_TYPE(Engine::Audio::OpenALAudioContext)