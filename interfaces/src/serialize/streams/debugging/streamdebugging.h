#pragma once

namespace Engine {
	namespace Serialize {
		namespace Debugging {

			class StreamLog {
			public:
				StreamLog(Stream *stream);

				void logRead(const ValueType &v);
				void logWrite(const ValueType &v);
				void logBeginMessage(const MessageHeader &header, SerializableUnitBase *unit = nullptr);
			private:
				std::ofstream mReads;
				std::ofstream mWrites;
			};

			class INTERFACES_EXPORT StreamDebugging {
			private:
				StreamDebugging();

			public:
				static void setLoggingEnabled(bool b);
				static bool isLoggingEnabled();

			private:
				static StreamDebugging sInstance;			
				bool mLoggingEnabled;
			};

		}
	}
}