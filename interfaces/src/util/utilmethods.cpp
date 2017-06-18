#include "interfaceslib.h"

#include <iostream>

namespace Engine {
	namespace Util {
		std::list<LogListener*> UtilMethods::sListeners;
		std::map<const std::exception*, std::list<TraceBack>> UtilMethods::sExceptions;
		std::list<TraceBack> UtilMethods::sCurrentTraceBack;
		std::list<TraceBack> UtilMethods::sExceptionTraceBack;
		std::list<TraceBack> UtilMethods::sTraceBack;
		Log *UtilMethods::sLog = 0;
		bool UtilMethods::sLogToStdCout = true;

		void UtilMethods::setup(Log *log, bool logToStdCout) {
			sLog = log;
			sLogToStdCout = logToStdCout;
		}

		void UtilMethods::log(const std::string & msg, MessageType level, const std::list<TraceBack> &traceBack)
		{
			sCurrentTraceBack = traceBack.empty() ? sTraceBack : traceBack;
			if (sLog)
				sLog->log(msg.c_str(), level, sCurrentTraceBack);
			else if (sLogToStdCout)
				std::cout << msg << std::endl;
			for (LogListener *listener : sListeners) {
				listener->messageLogged(msg, level, traceBack, sLog ? sLog->getName() : "Stdout");
			}

		}

		void UtilMethods::pushTraceBack(const TraceBack &t)
		{
			sTraceBack.push_back(t);
		}

		void UtilMethods::logException(const std::exception & e)
		{
			log(e.what(), ERROR_TYPE, sExceptionTraceBack);
			sExceptionTraceBack.clear();
		}

		void UtilMethods::popTraceBack()
		{
			sTraceBack.pop_back();
		}

		void UtilMethods::registerException(const TraceBack & t)
		{
			registerException();
			if (sExceptionTraceBack.empty()
				|| sExceptionTraceBack.back().mFile != t.mFile
				|| sExceptionTraceBack.back().mFunction != t.mFunction
				|| sExceptionTraceBack.back().mLineNr != -1)
				sExceptionTraceBack.push_back(t);
			else
				sExceptionTraceBack.back().mLineNr = t.mLineNr;
		}

		void UtilMethods::registerException()
		{
			sExceptionTraceBack = sTraceBack;
		}

		void UtilMethods::abort()
		{
			throw AbortException();
		}

		const std::list<TraceBack> &UtilMethods::traceBack() {
			return sCurrentTraceBack;
		}

		void UtilMethods::addListener(LogListener * listener)
		{
			sListeners.push_back(listener);
		}

		void UtilMethods::removeListener(LogListener * listener)
		{
			sListeners.remove(listener);
		}

	}
}