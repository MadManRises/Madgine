#include "../interfaceslib.h"

#include <iostream>

#include "abortexception.h"

#include "log.h"
#include "loglistener.h"

namespace Engine
{
	namespace Util
	{

		thread_local std::list<TraceBack> UtilMethods::__currentLogHolder::sCurrentTraceBack;
		thread_local std::list<TraceBack> UtilMethods::__currentLogHolder::sExceptionTraceBack;
		thread_local std::list<TraceBack> UtilMethods::__currentLogHolder::sTraceBack;
		thread_local Log* UtilMethods::__currentLogHolder::sLog = nullptr;
		thread_local bool UtilMethods::__currentLogHolder::sLogToStdCout = true;

		void UtilMethods::setup(Log* log)
		{
			__currentLogHolder::sLog = log;
		}


		void UtilMethods::setup(Log* log, bool logToStdCout)
		{
			__currentLogHolder::sLog = log;
			__currentLogHolder::sLogToStdCout = logToStdCout;
		}

		void UtilMethods::log(const std::string& msg, MessageType level, const std::list<TraceBack>& traceBack)
		{
			__currentLogHolder::sCurrentTraceBack = traceBack.empty() ? __currentLogHolder::sTraceBack : traceBack;
			if (__currentLogHolder::sLog)
				__currentLogHolder::sLog->log(msg.c_str(), level, __currentLogHolder::sCurrentTraceBack);
			else if (__currentLogHolder::sLogToStdCout)
				std::cout << msg << std::endl;
		}

		void UtilMethods::pushTraceBack(const TraceBack& t)
		{
			__currentLogHolder::sTraceBack.push_back(t);
		}

		void UtilMethods::logException(const std::exception& e)
		{
			log(e.what(), ERROR_TYPE, __currentLogHolder::sExceptionTraceBack);
			__currentLogHolder::sExceptionTraceBack.clear();
		}

		void UtilMethods::popTraceBack()
		{
			__currentLogHolder::sTraceBack.pop_back();
		}

		void UtilMethods::registerException(const TraceBack& t)
		{
			registerException();
			if (__currentLogHolder::sExceptionTraceBack.empty()
				|| __currentLogHolder::sExceptionTraceBack.back().mFile != t.mFile
				|| __currentLogHolder::sExceptionTraceBack.back().mFunction != t.mFunction
				|| __currentLogHolder::sExceptionTraceBack.back().mLineNr != -1)
				__currentLogHolder::sExceptionTraceBack.push_back(t);
			else
				__currentLogHolder::sExceptionTraceBack.back().mLineNr = t.mLineNr;
		}

		void UtilMethods::registerException()
		{
			__currentLogHolder::sExceptionTraceBack = __currentLogHolder::sTraceBack;
		}

		void UtilMethods::abort()
		{
			throw AbortException();
		}

		const std::list<TraceBack>& UtilMethods::traceBack()
		{
			return __currentLogHolder::sCurrentTraceBack;
		}

	}
}
