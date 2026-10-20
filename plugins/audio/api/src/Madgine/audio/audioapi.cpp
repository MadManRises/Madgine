#include "../audiolib.h"

#include "audioapi.h"

#include "Generic/execution/binding.h"
#include "Generic/execution/execution.h"

#include "Meta/math/pi.h"
#include "Meta/math/vector2.h"

#include "Madgine/behavior/nativebehaviorcollector.h"
#include "Madgine/root/root.h"

#include "Meta/reflect/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Audio::AudioApi)
METATABLE_END(Engine::Audio::AudioApi)

VIRTUALUNIQUECOMPONENTBASE(Engine::Audio::AudioApi)

METATABLE_BEGIN(Engine::Audio::AudioOptions)
    STORAGEOPS_BEGIN(Engine::Audio::AudioOptions, Engine::Audio::AudioOptions)
    CONSTRUCTOR()
    STORAGEOPS_END(Engine::Audio::AudioOptions)
    MEMBER(mVolume)
    MEMBER(mPan)
    MEMBER(mLooping)
METATABLE_END(Engine::Audio::AudioOptions)

SERIALIZETABLE_BEGIN(Engine::Audio::AudioOptions)
    FIELD(mVolume)
    FIELD(mPan)
    FIELD(mLooping)
SERIALIZETABLE_END(Engine::Audio::AudioOptions)

namespace Engine {
namespace Audio {

    AudioApi::AudioApi(Core::Root &root)
        : VirtualRootComponentBase(root)
    {
    }

    Behavior::Behavior AudioApi::playSound(std::string_view name, const AudioOptions &options)
    {
        return playSound(AudioLoader::load(name), options);
    }

    Behavior::Behavior AudioApi::playSound(AudioLoader::Handle buffer, const AudioOptions &options)
    {
        return playDynamicSound(std::move(buffer), Execution::BindingPtr<Math::Vector2> { Execution::ConstantBinding { Math::Vector2 { 0.0f, 0.0f } } }, options);
    }

    Behavior::Behavior AudioApi::playSound(const AudioBuffer &buffer, const AudioOptions &options)
    {
        return playDynamicSound(buffer, Execution::BindingPtr<Math::Vector2> { Execution::ConstantBinding { Math::Vector2 { 0.0f, 0.0f } } }, options);
    }

    Behavior::Behavior AudioApi::playDynamicSound(std::string_view name, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options)
    {
        return playDynamicSound(AudioLoader::load(name), distance, options);
    }

    int AudioApi::process(void *&output, OutputFormat format, const int16_t *&source, const void *end, size_t frameCount, size_t channels, const Execution::BindingPtr<Math::Vector2> &direction, const AudioOptions &options)
    {

        unsigned long count = std::min<unsigned int>(frameCount, (static_cast<const int16_t *>(end) - source) / channels);

        float volume = options.mVolume;

        float channelVolumes[8];
        if (channels == 2) {
            float pan = Math::clamp(options.mPan, -1.0f, 1.0f);
            channelVolumes[0] = Math::sqrtf((1.0f - pan) / 2.0f);
            channelVolumes[1] = Math::sqrtf((1.0f + pan) / 2.0f);
        } else {
            for (size_t i = 0; i < channels; ++i) {
                channelVolumes[i] = 1.0f;
            }
        }

        float distancePan = 0.0f;
        constexpr float halfPI = Math::PI / 2.0f;

        if (!Execution::access_binding(direction, [&](const Math::Vector2 &dir) {
                volume *= 2.0f / std::log2f(dir.squaredLength() + 4.0f);
                if (dir.x != 0.0f || dir.y != 0.0f)
                    distancePan = Math::clamp(std::atan2(dir.x, dir.y), -halfPI, halfPI) / halfPI;
            }))
            return -1;

        switch (format) {
        case I16: {

            int16_t *target = static_cast<int16_t *>(output);

            for (size_t i = 0; i < count; ++i) {

                if (channels == 2) {

                    int16_t left = *source++;
                    int16_t right = *source++;

                    *target++ = static_cast<int16_t>(channelVolumes[0] * volume * (left * std::max(0.0f, 1.0f - distancePan) + right * std::max(0.0f, -distancePan)));
                    *target++ = static_cast<int16_t>(channelVolumes[1] * volume * (right * std::max(0.0f, 1.0f + distancePan) + left * std::max(0.0f, -distancePan)));
                } else {

                    for (int j = 0; j < channels; ++j)
                        *target++ = static_cast<int16_t>(std::round(channelVolumes[j] * volume * *source++));
                }
            }
            break;
        }
        case F: {
            float *target = static_cast<float *>(output);

            for (size_t i = 0; i < count; ++i) {

                if (channels == 2) {

                    float left = *source++ / 32768.0f;
                    float right = *source++ / 32768.0f;

                    *target++ = channelVolumes[0] * volume * (left * std::max(0.0f, 1.0f - distancePan) + right * std::max(0.0f, -distancePan));
                    *target++ = channelVolumes[1] * volume * (right * std::max(0.0f, 1.0f + distancePan) + left * std::max(0.0f, -distancePan));
                } else {

                    for (int j = 0; j < channels; ++j)
                        *target++ = std::round(channelVolumes[j] * volume * (*source++ / 32768.0f));
                }
            }
            break;
        }
        default:
            throw 0;
        }

        return count;
    }

}
}

NATIVE_BEHAVIOR(play_sound, Engine::Audio::play_sound, Engine::Behavior::InputParameter<"buffer", Engine::Audio::AudioLoader::Handle>, Engine::Behavior::InputParameter<"options", Engine::Audio::AudioOptions>)
