#pragma once

#include "serializestream.h"

namespace Engine
{
	namespace Serialize
	{
		class INTERFACES_EXPORT BufferedInStream : public SerializeInStream
		{
		public:
			BufferedInStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id);

			bool isMessageAvailable();

			buffered_streambuf* rdbuf();

			void readHeader(MessageHeader& header);

		private:
			buffered_streambuf& mBuffer;
			std::istream mIfs;
		};

		class INTERFACES_EXPORT BufferedOutStream : public SerializeOutStream
		{
		public:
			BufferedOutStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id);

			void beginMessage(SerializableUnitBase* unit, MessageType type);
			void beginMessage(Command cmd);
			void endMessage();

			int sendMessages();

			BufferedOutStream& operator<<(BufferedInStream& in);
			using SerializeOutStream::operator<<;

			template <class... _Ty>
			void writeCommand(Command cmd, const _Ty&... args)
			{
				beginMessage(cmd);

				using expander = bool[];
				(void)expander{
					false,
					(*this << args, false)...
				};

				endMessage();
			}

		private:
			buffered_streambuf& mBuffer;
			std::ostream mOfs;
		};

		class INTERFACES_EXPORT BufferedInOutStream :
			public BufferedOutStream,
			public BufferedInStream
		{
		public:
			BufferedInOutStream(buffered_streambuf& buffer, SerializeManager& mgr, ParticipantId id);

			StreamError error() const;
			bool isClosed();
			void close();

			explicit operator bool() const;

		private:
			buffered_streambuf& mBuffer;
		};
	}
} // namespace Scripting
