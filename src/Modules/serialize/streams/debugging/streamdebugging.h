#pragma once

namespace Engine
{
	namespace Serialize
	{
		namespace Debugging
		{
			MODULES_EXPORT void setLoggingEnabled(bool b);
			MODULES_EXPORT bool isLoggingEnabled();
			MODULES_EXPORT void setLoggingPath(const std::string& path);


			class StreamLog
			{
			public:
				StreamLog(SerializeInStream &stream);
				StreamLog(SerializeOutStream &stream);

				template <class T>
				void log(const T& v)
				{
					if (isLoggingEnabled())
						mLogger << std::setw(20) << typeid(T).name() << " " << v << std::endl;
				}

				void log(const EOLType &)
				{
					if (isLoggingEnabled())
						mLogger << std::setw(20) << "EOL" << std::endl;
				}

				void logBeginMessage(const MessageHeader& header, const std::string& object);

			private:
				std::ofstream mLogger;
			};

			
		}
	}
}
