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
				if (StreamDebugging::isLoggingEnabled())
					mReads << std::setw(20) << v.getTypeString() << " " << v.toString() << std::endl;
			}

			void StreamLog::logWrite(const ValueType & v)
			{
				if (StreamDebugging::isLoggingEnabled())
					mWrites << std::setw(20) << v.getTypeString() << " " << v.toString() << std::endl;
			}

			StreamDebugging::StreamDebugging() {
			}

			void StreamDebugging::setLoggingEnabled(bool b)
			{
				if (sInstance.mLoggingEnabled != b) {
					sInstance.mLoggingEnabled = b;
					if (b) {
						if (std::experimental::filesystem::exists("stream-logging"))
							std::experimental::filesystem::remove_all("stream-logging");
						std::experimental::filesystem::create_directory("stream-logging");
					}
				}
			}

			bool StreamDebugging::isLoggingEnabled()
			{
				return sInstance.mLoggingEnabled;
			}
		}
	}
}