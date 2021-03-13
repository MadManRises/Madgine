#pragma once

namespace Engine {
namespace Serialize {

struct VerifierCategory;

struct DefaultVerifier {
    using Category = VerifierCategory;
    
    static bool verify(void *, ParticipantId requester)
    {
        return true;
    }
};

template <auto f, typename R, typename T, typename... Args>
struct CustomVerifierImpl {
    using Category = VerifierCategory;

    static bool verify(void *, Args... args)
    {
        return f(args...);
    }
};

template <auto f, typename R, typename T, typename... Args>
struct ParentVerifierImpl {
    using Category = VerifierCategory;

    static bool verify(T *parent, Args... args)
    {
        return (parent->*f)(args...);
    }
};

template <auto f>
using ParentVerifier = typename FunctionCapture<ParentVerifierImpl, f>::type;
template <auto f>
using CustomVerifier = typename FunctionCapture<CustomVerifierImpl, f>::type;

}
}