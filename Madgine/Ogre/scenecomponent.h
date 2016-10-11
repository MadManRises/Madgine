#pragma once

#include "uniquecomponent.h"
#include "Ogre/scenelistener.h"
#include "App/contextmasks.h"
#include "Scripting/Datatypes/Serialize/serializable.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace OGRE {

class MADGINE_EXPORT BaseSceneComponent : public SceneListener, public Scripting::Serialize::Serializable{
public:
    virtual ~BaseSceneComponent() = default;

    BaseSceneComponent(float updateInterval = 0.f, App::ContextMask context = App::ContextMask::SceneContext);

    void update(float timeSinceLastFrame, App::ContextMask mask);

    virtual void init();
	virtual void finalize();

    virtual const char *componentName() = 0;

	void setEnabled(bool b);
	bool isEnabled();

protected:
    virtual void update(float);

	void setUpdateInterval(float interval);

    virtual void load(Scripting::Serialize::SerializeInStream &in);
    virtual void save(Scripting::Serialize::SerializeOutStream &out) const;

private:
    const App::ContextMask mContext;

    float mUpdateInterval;
    float mTimeBank;

	bool mEnabled;

};

template MADGINE_EXPORT class UniqueComponentCollector<BaseSceneComponent>;

template <class T>
class SceneComponent : public UniqueComponent<T, BaseSceneComponent>, public Scripting::GlobalAPI<T>{

public:
	using UniqueComponent::UniqueComponent;

    virtual const char *componentName() override {
        return strrchr(typeid(T).name(), ':') + 1;
    }

};


}
}


