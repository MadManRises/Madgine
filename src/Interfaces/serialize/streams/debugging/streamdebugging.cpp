#include "../../../interfaceslib.h"

#include "streamdebugging.h"

#include "../serializestream.h"

#include "../../../generic/valuetype.h"

#include "../../serializemanager.h"

#include "../../../filesystem/api.h"

namespace Engine
{
	namespace Serialize
	{
		namespace Debugging
		{
			StreamDebugging StreamDebugging::sInstance;

			StreamLog::StreamLog(Stream* stream) :
				mReads(
					StreamDebugging::getLoggingPath() + stream->manager().name() + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::
					to_string(stream->id()) + "r.log"),
				mWrites(
					StreamDebugging::getLoggingPath() + stream->manager().name() + "_" + (stream->isMaster() ? "m" : "s") + "_" + std::
					to_string(stream->id()) + "w.log")
			{
				if (stream->manager().name().empty())
					throw 0;
			}

			

			void StreamLog::logBeginSendMessage(const MessageHeader& header, const std::string& object)
			{
				logBeginMessage(header, object, mWrites);
			}

			void StreamLog::logBeginReadMessage(const MessageHeader& header, const std::string& object)
			{
				logBeginMessage(header, object, mReads);
			}

			void StreamLog::logBeginMessage(const MessageHeader& header, const std::string& object, std::ofstream& stream)
			{
				if (StreamDebugging::isLoggingEnabled())
				{
					if (header.mObject != SERIALIZE_MANAGER)
					{
						stream << "// Begin Message of Type ";
						switch (header.mType)
						{
						case ACTION:
							stream << "ACTION";
							break;
						case REQUEST:
							stream << "REQUEST";
							break;
						case STATE:
							stream << "STATE";
							break;
						}
						stream << " to " << object << "(" << header.mObject << ")" << std::endl;
					}
					else
					{
						stream << "// Begin Command of Type " << header.mCmd << " to Manager (" << object << ")" << std::endl;
					}
				}
			}

			StreamDebugging::StreamDebugging() : mLoggingEnabled(false),
			                                     mPath("stream-logging/")
			{
			}

			void StreamDebugging::setLoggingEnabled(bool b)
			{
				if (sInstance.mLoggingEnabled != b)
				{
					sInstance.mLoggingEnabled = b;
					if (b)
					{
						if (Filesystem::exists(sInstance.mPath))
						{
							for (const Filesystem::Path &p : Filesystem::listFilesRecursive(sInstance.mPath))
							{
								Filesystem::remove(p);
							}
						}
						else
						{
							Filesystem::createDirectories(sInstance.mPath);
						}
					}
				}
			}

			bool StreamDebugging::isLoggingEnabled()
			{
				return sInstance.mLoggingEnabled;
			}

			void StreamDebugging::setLoggingPath(const std::string& path)
			{
				assert(!sInstance.mLoggingEnabled);
				sInstance.mPath = path;
			}

			const std::string& StreamDebugging::getLoggingPath()
			{
				return sInstance.mPath;
			}
		}
	}
}
