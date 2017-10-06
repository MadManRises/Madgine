#pragma once

#include "serialize/container/list.h"

#include "scene/scenemanager.h"


#include "scene/light.h"

namespace Engine {
namespace Scene {

class MADGINE_SERVER_EXPORT ServerSceneManager :
	public SceneManager<ServerSceneManager>
{
public:
    ServerSceneManager();
    virtual ~ServerSceneManager();	

    virtual bool init() override;
	virtual void finalize() override;
	
	virtual Light * createLight() override;
	virtual std::list<Light*> lights() override;

	virtual void clear() override;

	virtual void writeState(Serialize::SerializeOutStream &out) const override;
	virtual void readState(Serialize::SerializeInStream &in) override;

	void readScene(Serialize::SerializeInStream &in);
	void writeScene(Serialize::SerializeOutStream &out) const;

	static constexpr const float sSceneRasterSize = .2f;

protected:
		
    
private:

	std::string mStaticSceneName;

	Serialize::ObservableList<Light, Serialize::ContainerPolicy::masterOnly> mLights;

};

}
}


