#pragma once


#include "Madgine/serialize/toplevelserializableunit.h"

#include "Madgine/serialize/container/action.h"

namespace Maditor {
	namespace Shared {

		class MADITOR_SHARED_EXPORT AppControl : public Engine::Serialize::TopLevelSerializableUnit<AppControl> {

		public:
			static const constexpr struct masterLauncher_t {} masterLauncher{};

			AppControl();
			AppControl(masterLauncher_t);			
			virtual ~AppControl() = default;

		protected:

			virtual void onApplicationConnected() = 0;
			virtual void shutdownImpl() = 0;
			virtual void startImpl() = 0;
			virtual void stopImpl() = 0;
			virtual void pauseImpl() = 0;
			virtual void configImpl(const ApplicationInfo &);
			virtual void resizeWindowImpl();
			virtual void pingImpl();
			virtual void execLuaImpl(const std::string &);
			virtual void luaResponseImpl(const std::string &, const std::string &, bool);

			void shutdownImpl2() {
				shutdownImpl();
			};
			void startImpl2() {
				startImpl();
			}
			void stopImpl2() {
				stopImpl();
			}
			void pauseImpl2() {
				pauseImpl();
			}
			void resizeWindowImpl2() {
				resizeWindowImpl();
			}
			void pingImpl2() {
				pingImpl();
			}
			void onApplicationConnected2() {
				onApplicationConnected();
			}

			void execLuaImpl2(const std::string &s) {
				execLuaImpl(s);
			}

			void configImpl2(const ApplicationInfo &i)
			{
				configImpl(i);
			}

			void luaResponseImpl2(const std::string &cmd, const std::string &response, bool failure)
			{
				luaResponseImpl(cmd, response, failure);
			}

			Engine::Serialize::Action<&AppControl::shutdownImpl2, Engine::Serialize::ActionPolicy::broadcast> shutdown;
			Engine::Serialize::Action<&AppControl::onApplicationConnected2, Engine::Serialize::ActionPolicy::broadcast> applicationConnected;
			Engine::Serialize::Action<&AppControl::startImpl2, Engine::Serialize::ActionPolicy::broadcast> start;
			Engine::Serialize::Action<&AppControl::stopImpl2, Engine::Serialize::ActionPolicy::broadcast> stop;
			Engine::Serialize::Action<&AppControl::pauseImpl2, Engine::Serialize::ActionPolicy::broadcast> pause;
			Engine::Serialize::Action<&AppControl::resizeWindowImpl2, Engine::Serialize::ActionPolicy::broadcast> resizeWindow;
			Engine::Serialize::Action<&AppControl::pingImpl2, Engine::Serialize::ActionPolicy::broadcast> ping;
			Engine::Serialize::Action<&AppControl::execLuaImpl2, Engine::Serialize::ActionPolicy::request> execLua;
			Engine::Serialize::Action<&AppControl::configImpl2, Engine::Serialize::ActionPolicy::request> configure;
			Engine::Serialize::Action<&AppControl::luaResponseImpl2, Engine::Serialize::ActionPolicy::notification> luaResponse;
			
		};


	}
}

