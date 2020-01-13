#pragma once

//#include "contextmasks.h"

#include "Modules/keyvalue/scopebase.h"
#include "Modules/madgineobject/madgineobject.h"
#include "Modules/serialize/serializableunit.h"

#include "Modules/reflection/classname.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Scene {

    class MADGINE_SCENE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public ScopeBase, public MadgineObject {
    public:
        virtual ~SceneComponentBase() = default;

        SceneComponentBase(SceneManager &sceneMgr);

        SceneManager &sceneMgr(bool = true) const;

        virtual void update(std::chrono::microseconds);

        const char *key() const;

        template <class T>
        T &getSceneComponent(bool init = true)
        {
            return static_cast<T &>(getSceneComponent(component_index<T>(), init));
        }

        SceneComponentBase &getSceneComponent(size_t i, bool = true);

        template <class T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);
        SceneComponentBase &getSelf(bool = true);

        virtual const MadgineObject *parent() const override;

    protected:
        virtual bool init() override;
        virtual void finalize() override;

    private:
        SceneManager &mSceneMgr;
    };

}
}