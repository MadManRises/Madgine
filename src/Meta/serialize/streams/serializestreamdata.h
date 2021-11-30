#pragma once

namespace Engine {
namespace Serialize {

	enum class StreamMode {
        READ,
        WRITE
    };

    struct META_EXPORT SerializeStreamData {
    public:
        SerializeStreamData(std::unique_ptr<Formatter> format);
        SerializeStreamData(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id);
        SerializeStreamData(SerializeStreamData &&) = delete;        
        virtual ~SerializeStreamData();

        void setManager(SerializeManager *mgr);
        SerializeManager *manager();
        bool isMaster(StreamMode mode);

        ParticipantId id() const;
        void setId(ParticipantId id);

        Formatter &format() const;

        SerializableUnitList &serializableList();
        friend struct SerializableListHolder;

        SerializableUnitMap &serializableMap();
        friend struct SerializableMapHolder;

    private:
        SerializeManager *mManager = nullptr;
        ParticipantId mId = 0;
        std::unique_ptr<Formatter> mFormatter;

        union {
            SerializableUnitMap *mSerializableMap = nullptr;
            SerializableUnitList *mSerializableList;
        };
    };

}
}