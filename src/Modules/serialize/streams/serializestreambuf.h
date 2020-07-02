#pragma once

namespace Engine {
namespace Serialize {

	enum class StreamMode {
        READ,
        WRITE
    };

    struct MODULES_EXPORT SerializeStreambuf : std::basic_streambuf<char> {
    public:
        SerializeStreambuf(std::unique_ptr<Formatter> format);
        SerializeStreambuf(SerializeStreambuf &&);
        SerializeStreambuf(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id);
        virtual ~SerializeStreambuf();

        void setManager(SerializeManager *mgr);
        SerializeManager *manager();
        bool isMaster(StreamMode mode);

        ParticipantId id() const;
        void setId(ParticipantId id);

        Formatter &format() const;

    private:
        SerializeManager *mManager = nullptr;
        ParticipantId mId = 0;
        std::unique_ptr<Formatter> mFormatter;
    };

}
}