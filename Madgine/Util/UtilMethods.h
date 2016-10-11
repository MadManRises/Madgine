#pragma once

namespace Engine {
	namespace Util {

		class MADGINE_EXPORT UtilMethods {
		public:
			static void setup();

			

			struct TraceBack {
				TraceBack(const std::string &file = "<unknown>", int line = -1, const std::string &func = "") :
					mFile(file),
					mLineNr(line),
					mFunction(func) {}

				std::string mFile;
				int mLineNr;
				std::string mFunction;
			};

			static void log(const std::string &msg, Ogre::LogMessageLevel lvl, const std::list<TraceBack> &traceBack = {});
			
			static void popTraceBack();
			static void pushTraceBack(const TraceBack &t);

			static void logException(const std::exception &e);

			struct Tracer {
				Tracer(const TraceBack &t) {
					pushTraceBack(t);
				}
				~Tracer() {
					popTraceBack();
				}
			};

			static void registerException();
			
			static const std::list<TraceBack>& traceBack();
			
			
		private:
			static std::list<TraceBack> sCurrentTraceBack;
			static std::list<TraceBack> sTraceBack;
			static std::list<TraceBack> sExceptionTraceBack;
			static std::map<const std::exception *, std::list<TraceBack>> sExceptions;
			static const std::string sLogFileName;
			static Ogre::Log *sLog;
		};

#define TRACEBACK Engine::Util::UtilMethods::TraceBack(__FILE__, __LINE__, __FUNCTION__)
#define TRACE Engine::Util::UtilMethods::Tracer __t(TRACEBACK)
#define TRACE_S(FILE, LINE, FUNC) Engine::Util::UtilMethods::Tracer __t(Engine::Util::UtilMethods::TraceBack(FILE, LINE, FUNC));

#define LOG(s) (Engine::Util::UtilMethods::Tracer(TRACEBACK), Engine::Util::UtilMethods::log(s, Ogre::LML_NORMAL))
#define ERROR(s) (Engine::Util::UtilMethods::Tracer(TRACEBACK), Engine::Util::UtilMethods::log(s, Ogre::LML_CRITICAL))
#define LOG_EXCEPTION(e) Engine::Util::UtilMethods::logException(e);

#define MADGINE_THROW(e) throw (Engine::Util::UtilMethods::registerException(), e)

	}
}