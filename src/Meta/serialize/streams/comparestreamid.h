#pragma once

namespace Engine {
namespace Serialize {

    struct META_EXPORT CompareStreamId {
        bool operator()(const FormattedSerializeStream &first, const FormattedSerializeStream &second) const;
        bool operator()(const FormattedSerializeStream &first, ParticipantId second) const;
        bool operator()(ParticipantId first, const FormattedSerializeStream &second) const;

        using is_transparent = void;
    };
} // namespace Serialize
} // namespace Engine