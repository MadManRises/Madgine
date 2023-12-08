#pragma once

namespace Engine {

template <typename R, std::same_as<void> T, typename... Args>
struct CallableViewImpl {

    template <typename F>
    explicit CallableViewImpl(F &f)
        : mF([](void *context, Args &&...args) -> R {
            return (*static_cast<F *>(context))(std::forward<Args>(args)...);
        })
        , mContext(&f)
    {
    }

    R operator()(Args &&...args) const
    {
        return mF(mContext, std::forward<Args>(args)...);
    }

private:
    R (*mF)
    (void *, Args &&...);
    void *mContext;
};

template <typename Signature>
using CallableView = typename CallableTraits<Signature>::template instance<CallableViewImpl>::type;

}