#pragma once

#include "traceback.h"

namespace Engine
{
	namespace Util
	{
		class AbortException
		{
		};

		enum MessageType
		{
			ERROR_TYPE,
			WARNING_TYPE,
			LOG_TYPE
		};

		class Log
		{
		public:
			virtual ~Log() = default;
			virtual void log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack = {}) = 0;
			virtual std::string getName() = 0;
		};

		class LogListener
		{
		public:
			virtual ~LogListener() = default;
			virtual void messageLogged(const std::string& message, MessageType lml, const std::list<TraceBack>& traceback,
			                           const std::string& logName) = 0;
		};

		class INTERFACES_EXPORT UtilMethods
		{
		public:
			static void setup(Log* log = nullptr, bool logToStdCout = true);

			static void log(const std::string& msg, MessageType lvl, const std::list<TraceBack>& traceBack = {});

			static void popTraceBack();
			static void pushTraceBack(const TraceBack& t);

			static void logException(const std::exception& e);

			struct Tracer
			{
				Tracer(const TraceBack& t)
				{
					pushTraceBack(t);
				}

				~Tracer()
				{
					popTraceBack();
				}
			};

			static void registerException(const TraceBack& t);
			static void registerException();

			static void abort();

			static const std::list<TraceBack>& traceBack();

			static void addListener(LogListener* listener);
			static void removeListener(LogListener* listener);


		private:
			static std::list<TraceBack> sCurrentTraceBack;
			static std::list<TraceBack> sTraceBack;
			static std::list<TraceBack> sExceptionTraceBack;
			static std::list<LogListener*> sListeners;

			static Log* sLog;
			static bool sLogToStdCout;
		};

#define TRACEBACK Engine::Util::TraceBack(__FILE__, __LINE__, __FUNCTION__)
#define TRACE Engine::Util::UtilMethods::Tracer __t(TRACEBACK)
#define TRACE_S(FILE, LINE, FUNC) Engine::Util::UtilMethods::Tracer __t(Engine::Util::TraceBack(FILE, LINE, FUNC));

#define LOG(s) (Engine::Util::UtilMethods::Tracer(TRACEBACK), Engine::Util::UtilMethods::log(s, Engine::Util::LOG_TYPE))
#define LOG_WARNING(s) (Engine::Util::UtilMethods::Tracer(TRACEBACK), Engine::Util::UtilMethods::log(s, Engine::Util::WARNING_TYPE))
#define LOG_ERROR(s) (Engine::Util::UtilMethods::Tracer(TRACEBACK), Engine::Util::UtilMethods::log(s, Engine::Util::ERROR_TYPE))
#define LOG_EXCEPTION(e) Engine::Util::UtilMethods::logException(e);

#define MADGINE_THROW(e) throw (Engine::Util::UtilMethods::registerException(TRACEBACK), e)
#define MADGINE_THROW_NO_TRACE(e) throw (Engine::Util::UtilMethods::registerException(), e)

#define MADGINE_ABORT Engine::Util::UtilMethods::abort()
	}
}
