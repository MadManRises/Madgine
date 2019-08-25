#include "../../../moduleslib.h"

#include "streamdebugging.h"

#include "../serializestream.h"

#include "../../../keyvalue/valuetype.h"

#include "../../serializemanager.h"

#include "Interfaces/filesystem/api.h"

namespace Engine
{
	namespace Serialize
	{
		namespace Debugging
		{

			static bool sLoggingEnabled = false;
			static std::string sPath = "stream-logging/";

			StreamLog::StreamLog(SerializeInStream &stream) :
				mLogger(
					sPath + (stream.manager() ? stream.manager()->name() : "unnamed") + "_" + (stream.isMaster() ? "m" : "s") + "_" + std::
					to_string(stream.id()) + "r.log")
			{
                            assert(mLogger);
				if (stream.manager() && stream.manager()->name().empty())
					throw 0;
			}

			StreamLog::StreamLog(SerializeOutStream &stream) :
				mLogger(
					sPath + (stream.manager() ? stream.manager()->name() : "unnamed") + "_" + (stream.isMaster() ? "m" : "s") + "_" + std::
					to_string(stream.id()) + "w.log")
			{
                            assert(mLogger);
				if (stream.manager() && stream.manager()->name().empty())
					throw 0;
			}

			void StreamLog::logBeginMessage(const MessageHeader& header, const std::string& object)
			{
				if (sLoggingEnabled)
				{
					if (header.mObject != SERIALIZE_MANAGER)
					{
						mLogger << "// Begin Message of Type ";
						switch (header.mType)
						{
						case ACTION:
							mLogger << "ACTION";
							break;
						case REQUEST:
							mLogger << "REQUEST";
							break;
						case STATE:
							mLogger << "STATE";
							break;
						}
						mLogger << " to " << object << "(" << header.mObject << ")" << std::endl;
					}
					else
					{
						mLogger << "// Begin Command of Type " << header.mCmd << " to Manager (" << object << ")" << std::endl;
					}
				}
			}

			void setLoggingEnabled(bool b)
			{
				if (sLoggingEnabled != b)
				{
					sLoggingEnabled = b;
					if (b)
					{
						if (Filesystem::exists(sPath))
						{
							for (const Filesystem::Path &p : Filesystem::listFilesRecursive(sPath))
							{
								Filesystem::remove(p);
							}
						}
						else
						{
							Filesystem::createDirectories(sPath);
						}
					}
				}
			}

			bool isLoggingEnabled()
			{
				return sLoggingEnabled;
			}

			void setLoggingPath(const std::string& path)
			{
				assert(!sLoggingEnabled);
				sPath = path;
			}

		}
	}
}
