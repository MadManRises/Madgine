#include "serverlib.h"

#include "serverscenemanager.h"


#include "serialize/streams/serializestream.h"

namespace Engine
{
	namespace Scene
	{
		ServerSceneManager::ServerSceneManager()
		{
		}

		ServerSceneManager::~ServerSceneManager()
		{
		}

		bool ServerSceneManager::init()
		{
			return SceneManagerBase::init();
		}

		void ServerSceneManager::finalize()
		{
			SceneManagerBase::finalize();
		}


		void ServerSceneManager::writeState(Serialize::SerializeOutStream& out) const
		{
			out << mStaticSceneName << ValueType::EOL() << ValueType::EOL();

			SceneManagerBase::writeState(out);
		}

		void ServerSceneManager::readState(Serialize::SerializeInStream& in)
		{
			ValueType dummy;
			in >> mStaticSceneName >> dummy >> dummy;

			SceneManagerBase::readState(in);
		}

		void ServerSceneManager::readScene(Serialize::SerializeInStream& in)
		{
			in.process().startSubProcess(1, "Loading Level...");

			readState(in);

			applySerializableMap(in.manager().slavesMap());

			in.process().endSubProcess();
		}

		void ServerSceneManager::writeScene(Serialize::SerializeOutStream& out) const
		{
			writeState(out);
		}


		Light* ServerSceneManager::createLight()
		{
			return &*mLights.emplace_back().first;
		}

		std::list<Light*> ServerSceneManager::lights()
		{
			std::list<Light*> result;
			for (Light& light : mLights)
			{
				result.push_back(&light);
			}
			return result;
		}

		void ServerSceneManager::clear()
		{
			mLights.clear();

			SceneManagerBase::clear();
		}
	}
}
