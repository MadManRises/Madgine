#pragma once

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"

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
        ALCdevice *mDevice = nullptr;
        ALCcontext *mContext = nullptr;
    };

}
}

RegisterType(Engine::Audio::OpenALAudioContext);