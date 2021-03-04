#pragma once

namespace Engine {
namespace Serialize {

struct VerifierCategory;

struct DefaultVerifier {
    using Category = VerifierCategory;

    template <typename... Args>
    static bool verify(ParticipantId requester, Args &&... args)
    {
        return true;
    }
};

}
}