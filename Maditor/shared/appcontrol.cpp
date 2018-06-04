#include "maditorsharedlib.h"


#include "appcontrol.h"

#include "ipcmanager/boostipcmanager.h"

#include "sharedmemory.h"

#include "Madgine/serialize/toplevelids.h"

namespace Maditor {
	namespace Shared {

		AppControl::AppControl()
		{			
		}

		AppControl::AppControl(masterLauncher_t) :
			SerializableUnit(Engine::Serialize::MADITOR)
		{
		}

		void AppControl::configImpl(const ApplicationInfo&)
		{
		}

		void AppControl::resizeWindowImpl()
		{
		}

		void AppControl::pingImpl()
		{
		}

		void AppControl::execLuaImpl(const std::string &)
		{
		}

		void AppControl::luaResponseImpl(const std::string&, const std::string&, bool)
		{
		}
	}
}
