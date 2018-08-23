#pragma once

namespace Engine
{
	namespace Serialize
	{
		namespace Debugging
		{

			class INTERFACES_EXPORT StreamDebugging
			{
			private:
				StreamDebugging();

			public:
				static void setLoggingEnabled(bool b);
				static bool isLoggingEnabled();

				static void setLoggingPath(const std::string& path);
				static const std::string& getLoggingPath();

			private:
				static StreamDebugging sInstance;
				bool mLoggingEnabled;
				std::string mPath;
			};


			class StreamLog
			{
			public:
				StreamLog(Stream* stream);

				template <class T>
				void logRead(const T& v)
				{
					if (StreamDebugging::isLoggingEnabled())
						mReads << std::setw(20) << typeid(T).name() << " " << v << std::endl;
				}

				template <class T>
				void logWrite(const T& v)
				{
					if (StreamDebugging::isLoggingEnabled())
						mWrites << std::setw(20) << typeid(T).name() << " " << v << std::endl;
				}

				void logRead(const EOLType &)
				{
					if (StreamDebugging::isLoggingEnabled())
						mReads << std::setw(20) << "EOL" << std::endl;
				}

				void logWrite(const EOLType &)
				{
					if (StreamDebugging::isLoggingEnabled())
						mWrites << std::setw(20) << "EOL" << std::endl;
				}

				void logBeginSendMessage(const MessageHeader& header, const std::string& object);
				void logBeginReadMessage(const MessageHeader& header, const std::string& object);

			private:
				void logBeginMessage(const MessageHeader& header, const std::string& object, std::ofstream& stream);

			private:
				std::ofstream mReads;
				std::ofstream mWrites;
			};

			
		}
	}
}
