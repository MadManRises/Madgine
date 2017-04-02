#pragma once

#include "uniquecomponent.h"
#include "App/contextmasks.h"
#include "Serialize/serializableunit.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace Scene {

class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public MadgineObject{
public:
    virtual ~SceneComponentBase() = default;

    SceneComponentBase(App::ContextMask context = App::ContextMask::SceneContext);

    void update(float timeSinceLastFrame, App::ContextMask mask);
	void fixedUpdate(float timeStep, App::ContextMask mask);

    virtual bool init();
	virtual void finalize();    

	void setEnabled(bool b);
	bool isEnabled();

	SceneManagerBase *sceneMgr();

protected:
    virtual void update(float);
	virtual void fixedUpdate(float);    

private:
    const App::ContextMask mContext;

	bool mEnabled;

	SceneManagerBase *mSceneMgr;

};

#ifdef _MSC_VER
template MADGINE_BASE_EXPORT class BaseUniqueComponentCollector<SceneComponentBase>;
#endif

template <class T>
class SceneComponent : public UniqueComponent<T, SceneComponentBase>, public Scripting::GlobalAPI<T>{

public:
	using UniqueComponent::UniqueComponent;

private:
	virtual size_t getSize() const override final {
		return sizeof(T);
	}

};




}
}


