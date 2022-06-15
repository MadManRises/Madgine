#pragma once

#include "Meta/keyvalue/virtualscope.h"
#include "madgineobject/madgineobject.h"
#include "Meta/serialize/hierarchy/syncableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Scene {

    struct MADGINE_SCENE_EXPORT SceneComponentBase : VirtualScopeBase<>, Serialize::SyncableUnitBase, MadgineObject<SceneComponentBase> {    
        virtual ~SceneComponentBase() = default;

        SceneComponentBase(SceneManager &sceneMgr);

        void sceneMgr(SceneManager *&ptr) const;
        SceneManager &sceneMgr() const;

        virtual void update(std::chrono::microseconds, bool paused);

        std::string_view key() const;

template <typename T>
        T &getSceneComponent()
        {
            return static_cast<T &>(getSceneComponent(component_index<T>()));
        }

        SceneComponentBase &getSceneComponent(size_t i);

        template <typename T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i);

        Threading::TaskQueue *taskQueue() const;

    protected:
        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        friend struct MadgineObject<SceneComponentBase>;

    private:
        SceneManager &mSceneMgr;
    };

}
}

REGISTER_TYPE(Engine::Scene::SceneComponentBase)