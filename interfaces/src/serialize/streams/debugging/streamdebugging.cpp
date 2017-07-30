#include "interfaceslib.h"

#include "streamdebugging.h"

#include "serialize/streams/serializestream.h"

#include "valuetype.h"

namespace Engine {
	namespace Serialize {
		namespace Debugging {

			StreamDebugging StreamDebugging::sInstance;

			StreamLog::StreamLog(Stream * stream) :
				mWrites(std::string("stream-logging/") + std::to_string(reinterpret_cast<uintptr_t>(&stream->manager())) + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::to_string(stream->id()) + "w.log"),
				mReads(std::string("stream-logging/") + std::to_string(reinterpret_cast<uintptr_t>(&stream->manager())) + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::to_string(stream->id()) + "r.log")
			{				
			}

			void StreamLog::logRead(const ValueType & v)
			{
				mReads << std::setw(20) << v.getTypeString() << " " << v.toString() << std::endl;
			}

			void StreamLog::logWrite(const ValueType & v)
			{
				mWrites << std::setw(20) << v.getTypeString() << " " << v.toString() << std::endl;
			}

			StreamDebugging::StreamDebugging() {
				std::experimental::filesystem::remove_all("stream-logging");
				std::experimental::filesystem::create_directory("stream-logging");
			}

			void StreamDebugging::setLoggingEnabled(bool b)
			{
				sInstance.mLoggingEnabled = b;
			}

			bool StreamDebugging::isLoggingEnabled()
			{
				return sInstance.mLoggingEnabled;
			}
		}
	}
}