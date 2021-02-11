#pragma once

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Meta/keyvalue/virtualscope.h"

struct ALCdevice;
struct ALCcontext;

namespace Engine {
namespace Audio {

    struct MADGINE_OPENAL_EXPORT OpenALAudioContext : public App::GlobalAPI<OpenALAudioContext> {
        OpenALAudioContext(App::Application &app);
        ~OpenALAudioContext();

        virtual bool init() override;
        virtual void finalize() override;

    private:
        ALCdevice *mDevice = nullptr;
        ALCcontext *mContext = nullptr;
    };

}
}

RegisterType(Engine::Audio::OpenALAudioContext);