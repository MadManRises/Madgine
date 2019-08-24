#pragma once

#include "scenecomponentcollector.h"

#include "Modules/serialize/container/syncablecontainer.h"

#include "light.h"

namespace Engine {
namespace Scene {

    class MADGINE_BASE_EXPORT LightManager : public VirtualSceneComponentBase<LightManager> {
    public:
        using VirtualSceneComponentBase<LightManager>::VirtualSceneComponentBase;
        LightManager(const LightManager &) = delete;
        virtual ~LightManager();

        Light *createLight();
        const std::list<std::unique_ptr<Light>> &lights() const;

        void finalize() override;
        void clear();

        virtual std::unique_ptr<Light> createLightImpl() = 0;

    private:
        std::tuple<std::unique_ptr<Light>> createLightTuple();

    private:
        Serialize::SyncableContainer<::Engine::Serialize::CombinedOffsetPtr<Self, __LINE__>, std::list<std::unique_ptr<Light>>, Serialize::ContainerPolicies::masterOnly> mLights;        DEFINE_COMBINED_OFFSET(mLights);
    };

}
}
