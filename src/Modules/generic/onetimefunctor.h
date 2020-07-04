#pragma once

namespace Engine {

template <typename F>
struct OneTimeFunctor {
    OneTimeFunctor(F &&f)
        : mF(std::forward<F>(f))
    {
    }

    OneTimeFunctor(OneTimeFunctor<F> &&other) noexcept
        : mF(std::move(other.mF))
    {        
        other.mF.reset();
    }

    OneTimeFunctor(const OneTimeFunctor<F> &other)
        : mF(std::move(other.mF))                
    {
        other.mF.reset();
    }

    template <typename... Args>
    decltype(auto) operator()(Args&&... args)
    {
        assert(mF);
        F f = *std::move(mF);
        mF.reset();
        return std::move(f)(std::forward<Args>(args)...);
    }

    operator bool() const {
        return mF.has_value();
    }

private:    
    mutable std::optional<F> mF;    
};

template <typename F>
auto oneTimeFunctor(F &&f)
{
    static_assert(!std::is_reference_v<F>);
    return OneTimeFunctor<F> { std::forward<F>(f) };
}

}