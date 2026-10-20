#pragma once

#include "Madgine/behavior/behavior.h"
#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"

#include "audioloader.h"

namespace Engine {
namespace Audio {

    ENUM_BASE(AudioResult, GenericResult);

    struct AudioOptions {
        float mVolume = 1.0f;
        float mPan = 0.0f;

        bool mLooping = false;
    };

    struct MADGINE_AUDIO_EXPORT AudioApi : Core::VirtualRootComponentBase<AudioApi> {

        AudioApi(Core::Root &root);

        Behavior::Behavior playSound(std::string_view name, const AudioOptions &options = {});
        Behavior::Behavior playSound(AudioLoader::Handle buffer, const AudioOptions &options = {});
        Behavior::Behavior playSound(const AudioBuffer &buffer, const AudioOptions &options = {});

        Behavior::Behavior playDynamicSound(std::string_view name, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options = {});
        virtual Behavior::Behavior playDynamicSound(AudioLoader::Handle buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options = {}) = 0;
        virtual Behavior::Behavior playDynamicSound(const AudioBuffer &buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options = {}) = 0;

        enum OutputFormat {
            I16,
            F
        };

        static int process(void *&output, OutputFormat format, const int16_t *&source, const void *end, size_t frameCount, size_t channels, const Execution::BindingPtr<Math::Vector2> &direction, const AudioOptions &options);
    };

    template <typename T>
    using AudioApiImpl = Reflect::VirtualScope<T, Core::RootComponentVirtualImpl<T, AudioApi>>;

    inline Behavior::Behavior play_sound(AudioLoader::Handle buffer, const AudioOptions &options = {})
    {
        return AudioApi::getSingleton().playSound(std::move(buffer), options);
    };

    inline Behavior::Behavior play_sound(const AudioBuffer &buffer, const AudioOptions &options = {})
    {
        return AudioApi::getSingleton().playSound(buffer, options);
    };

    inline Behavior::Behavior play_dynamic_sound(AudioLoader::Handle buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options = {})
    {
        return AudioApi::getSingleton().playDynamicSound(std::move(buffer), std::move(distance), options);
    };

    inline Behavior::Behavior play_dynamic_sound(const AudioBuffer &buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options = {})
    {
        return AudioApi::getSingleton().playDynamicSound(buffer, std::move(distance), options);
    };

}
}