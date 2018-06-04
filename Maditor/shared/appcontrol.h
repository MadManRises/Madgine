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

			Engine::Serialize::Action<decltype(&AppControl::shutdownImpl2), &AppControl::shutdownImpl2, Engine::Serialize::ActionPolicy::broadcast> shutdown;
			Engine::Serialize::Action<decltype(&AppControl::onApplicationConnected2), &AppControl::onApplicationConnected2, Engine::Serialize::ActionPolicy::broadcast> applicationConnected;
			Engine::Serialize::Action<decltype(&AppControl::startImpl2), &AppControl::startImpl2, Engine::Serialize::ActionPolicy::broadcast> start;
			Engine::Serialize::Action<decltype(&AppControl::stopImpl2), &AppControl::stopImpl2, Engine::Serialize::ActionPolicy::broadcast> stop;
			Engine::Serialize::Action<decltype(&AppControl::pauseImpl2), &AppControl::pauseImpl2, Engine::Serialize::ActionPolicy::broadcast> pause;
			Engine::Serialize::Action<decltype(&AppControl::resizeWindowImpl2), &AppControl::resizeWindowImpl2, Engine::Serialize::ActionPolicy::broadcast> resizeWindow;
			Engine::Serialize::Action<decltype(&AppControl::pingImpl2), &AppControl::pingImpl2, Engine::Serialize::ActionPolicy::broadcast> ping;
			Engine::Serialize::Action<decltype(&AppControl::execLuaImpl2), &AppControl::execLuaImpl2, Engine::Serialize::ActionPolicy::request> execLua;
			Engine::Serialize::Action<decltype(&AppControl::configImpl2), &AppControl::configImpl2, Engine::Serialize::ActionPolicy::request> configure;
			Engine::Serialize::Action<decltype(&AppControl::luaResponseImpl2), &AppControl::luaResponseImpl2, Engine::Serialize::ActionPolicy::notification> luaResponse;
			
		};


	}
}

