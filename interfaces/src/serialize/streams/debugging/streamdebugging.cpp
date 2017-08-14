#include "interfaceslib.h"

#include "streamdebugging.h"

#include "serialize/streams/serializestream.h"

#include "valuetype.h"

#include "serialize/serializemanager.h"

namespace Engine {
	namespace Serialize {
		namespace Debugging {

			StreamDebugging StreamDebugging::sInstance;

			StreamLog::StreamLog(Stream * stream) :
				mReads(std::string("stream-logging/") + stream->manager().name() + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::to_string(stream->id()) + "r.log"),
				mWrites(std::string("stream-logging/") + stream->manager().name() + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::to_string(stream->id()) + "w.log")
			{		
				if (stream->manager().name().empty())
					throw 0;
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

			void StreamLog::logBeginSendMessage(const MessageHeader &header, const std::string &object) {
				if (StreamDebugging::isLoggingEnabled()) {
					mWrites << "// Begin Message of Type ";
					switch (header.mType) {
					case ACTION:
						mWrites << "ACTION";
						break;
					case REQUEST:
						mWrites << "REQUEST";
						break;
					case STATE:
						mWrites << "STATE";
						break;
					}
					mWrites << " to " << object << "(" << header.mObject << ")" << std::endl;
				}
			}

			void StreamLog::logBeginReadMessage(const MessageHeader &header, const std::string &object) {
				if (StreamDebugging::isLoggingEnabled()) {
					mReads << "// Begin Message of Type ";
					switch (header.mType) {
					case ACTION:
						mReads << "ACTION";
						break;
					case REQUEST:
						mReads << "REQUEST";
						break;
					case STATE:
						mReads << "STATE";
						break;
					}
					mReads << " to " << object << "(" << header.mObject << ")" << std::endl;
				}
			}

			StreamDebugging::StreamDebugging() {
			}

			void StreamDebugging::setLoggingEnabled(bool b)
			{
				if (sInstance.mLoggingEnabled != b) {
					sInstance.mLoggingEnabled = b;
					if (b) {
						if (std::experimental::filesystem::exists("stream-logging")) {
							for (std::experimental::filesystem::directory_iterator end, it("stream-logging"); it != end; ++it) {
								std::experimental::filesystem::remove_all(it->path());
							}
						}
						else {
							std::experimental::filesystem::create_directory("stream-logging");
						}
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