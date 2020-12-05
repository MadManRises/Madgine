#pragma once

#include "Modules/keyvalue/virtualscopebase.h"
#include "Modules/madgineobject/madgineobject.h"
#include "Modules/serialize/syncableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Scene {

    struct MADGINE_SCENE_EXPORT SceneComponentBase : VirtualScopeBase, Serialize::SyncableUnitBase, MadgineObject<SceneComponentBase> {    
        virtual ~SceneComponentBase() = default;

        SceneComponentBase(SceneManager &sceneMgr);

        SceneManager &sceneMgr(bool = true) const;

        virtual void update(std::chrono::microseconds);

        std::string_view key() const;

        template <typename T>
        T &getSceneComponent(bool init = true)
        {
            return static_cast<T &>(getSceneComponent(component_index<T>(), init));
        }

        SceneComponentBase &getSceneComponent(size_t i, bool = true);

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        const SceneManager *parent() const;

    protected:
        virtual bool init();
        virtual void finalize();

        friend struct MadgineObject<SceneComponentBase>;

    private:
        SceneManager &mSceneMgr;
    };

}
}

RegisterType(Engine::Scene::SceneComponentBase);