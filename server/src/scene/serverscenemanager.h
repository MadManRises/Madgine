#pragma once

#include "serialize/container/list.h"

#include "scene/scenemanager.h"


#include "scene/light.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_SERVER_EXPORT ServerSceneManager :
			public SceneManager<ServerSceneManager>
		{
		public:
			ServerSceneManager();
			virtual ~ServerSceneManager();

			bool init() override;
			void finalize() override;

			Light* createLight() override;
			std::list<Light*> lights() override;

			void clear() override;

			void writeState(Serialize::SerializeOutStream& out) const override;
			void readState(Serialize::SerializeInStream& in) override;

			void readScene(Serialize::SerializeInStream& in);
			void writeScene(Serialize::SerializeOutStream& out) const;

			static constexpr const float sSceneRasterSize = .2f;


		private:

			std::string mStaticSceneName;

			Serialize::ObservableList<Light, Serialize::ContainerPolicies::masterOnly> mLights;
		};
	}
}
