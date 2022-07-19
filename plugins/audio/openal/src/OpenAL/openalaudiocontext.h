#pragma once

#include "Madgine/base/globalapibase.h"
#include "Madgine/base/globalapicollector.h"

#include "Generic/functor.h"

struct ALCdevice;
struct ALCcontext;

namespace Engine {
namespace Audio {

    struct MADGINE_OPENAL_EXPORT OpenALAudioContext : public Base::GlobalAPI<OpenALAudioContext> {
        OpenALAudioContext(Base::Application &app);
        ~OpenALAudioContext();

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual std::string_view key() const override;

    private:
        std::unique_ptr<ALCdevice, Functor<&alcCloseDevice>> mDevice;
        std::unique_ptr<ALCcontext, Functor<&alcDestroyContext>> mContext;
    };

}
}

REGISTER_TYPE(Engine::Audio::OpenALAudioContext)