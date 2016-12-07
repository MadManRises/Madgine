#pragma once

#include "uniquecomponent.h"
#include "scenelistener.h"
#include "App/contextmasks.h"
#include "Serialize/serializableunit.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace Scene {

class MADGINE_EXPORT BaseSceneComponent : public SceneListener, public Serialize::SerializableUnit{
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

    

private:
    const App::ContextMask mContext;

    float mUpdateInterval;
    float mTimeBank;

	bool mEnabled;

};

#ifdef _MSC_VER
template MADGINE_EXPORT class UniqueComponentCollector<BaseSceneComponent>;
#endif

template <class T>
class SceneComponent : public UniqueComponent<T, BaseSceneComponent>, public Scripting::GlobalAPI<T>{

public:
	using UniqueComponent<T, BaseSceneComponent>::UniqueComponent;

    virtual const char *componentName() override {
        return strrchr(typeid(T).name(), ':') + 1;
    }

};




}
}


