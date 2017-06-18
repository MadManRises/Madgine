#pragma once



namespace Engine {
	namespace Util {

		class INTERFACES_EXPORT StandardLog : public Log {
		public:
			StandardLog(const std::string &name);

			// Inherited via Log
			virtual void log(const std::string & msg, Engine::Util::MessageType lvl, const std::list<Engine::Util::TraceBack>& traceBack = {}) override;
			virtual std::string getName() override;

		private:
			std::string mName;
		};

	}
}