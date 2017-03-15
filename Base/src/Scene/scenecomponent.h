#pragma once

#include "uniquecomponent.h"
#include "App/contextmasks.h"
#include "Serialize/serializableunit.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace Scene {

class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public MadgineObjectBase{
public:
    virtual ~SceneComponentBase() = default;

    SceneComponentBase(float updateInterval = 0.f, App::ContextMask context = App::ContextMask::SceneContext);

    void update(float timeSinceLastFrame, App::ContextMask mask);

    virtual bool init();
	virtual void finalize();

    virtual const char *getName() = 0;

	void setEnabled(bool b);
	bool isEnabled();

	SceneManager *sceneMgr();

protected:
    virtual void update(float);

	void setUpdateInterval(float interval);

    

private:
    const App::ContextMask mContext;

    float mUpdateInterval;
    float mTimeBank;

	bool mEnabled;

	SceneManager *mSceneMgr;

};

#ifdef _MSC_VER
template MADGINE_BASE_EXPORT class UniqueComponentCollector<SceneComponentBase>;
#endif

template <class T>
class SceneComponent : public Hierarchy::HierarchyObject<T>, public UniqueComponent<T, SceneComponentBase>, public Scripting::GlobalAPI<T>{

public:
	using UniqueComponent<T, SceneComponentBase>::UniqueComponent;

    virtual const char *getName() override {
        return strrchr(typeid(T).name(), ':') + 1;
    }

};




}
}


