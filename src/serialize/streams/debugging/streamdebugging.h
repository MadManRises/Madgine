#pragma once

namespace Engine
{
	namespace Serialize
	{
		namespace Debugging
		{
			class StreamLog
			{
			public:
				StreamLog(Stream* stream);

				void logRead(const ValueType& v);
				void logWrite(const ValueType& v);
				void logBeginSendMessage(const MessageHeader& header, const std::string& object);
				void logBeginReadMessage(const MessageHeader& header, const std::string& object);

			private:
				void logBeginMessage(const MessageHeader& header, const std::string& object, std::ofstream& stream);

			private:
				std::ofstream mReads;
				std::ofstream mWrites;
			};

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
		}
	}
}
