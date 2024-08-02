#pragma once

#include "Madgine/root/root.h"

namespace Engine {
namespace Resources {

    struct with_handle_t {

        template <typename Rec, typename Handle>
        struct receiver : Execution::algorithm_receiver<Rec> {
            Handle mHandle;
        };

        template <typename Sender, typename Handle, typename Rec>
        struct state : Execution::base_state<receiver<Rec, Handle>> {

            using State = Execution::connect_result_t<Sender, receiver<Rec, Handle> &>;

            state(Sender &&sender, Rec &&rec, Handle &&handle)
                : Execution::base_state<receiver<Rec, Handle>> { { std::forward<Rec>(rec), std::forward<Handle>(handle) } }
                , mState(Execution::connect(std::forward<Sender>(sender), this->mRec))
            {
            }

            void start()
            {
                if (this->mRec.mHandle.info()->loadingTask().is_ready()) {
                    mState.start();
                } else {
                    Root::Root::getSingleton().taskQueue()->queueTask(this->mRec.mHandle.info()->loadingTask().then([this](bool success) {
                        if (success)
                            mState.start();
                        else
                            this->mRec.set_error(GenericResult { GenericResult::UNKNOWN_ERROR });
                    }));
                }
            }

            State mState;
        };

        template <typename Sender, typename Handle>
        struct sender : Execution::algorithm_sender<Sender> {

            template <typename Rec>
            friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
            {
                return state<Sender, Handle, Rec> { std::forward<Sender>(sender.mSender), std::forward<Rec>(rec), std::forward<Handle>(sender.mHandle) };
            }

            Handle mHandle;
        };

        template <typename Sender, typename Handle>
        friend auto tag_invoke(with_handle_t, Sender &&inner, Handle &&handle)
        {
            return sender<Sender, Handle> { { {}, std::forward<Sender>(inner) }, std::forward<Handle>(handle) };
        }

        template <typename Sender, typename Handle>
        requires tag_invocable<with_handle_t, Sender, Handle>
        auto operator()(Sender &&sender, Handle &&handle) const
            noexcept(is_nothrow_tag_invocable_v<with_handle_t, Sender, Handle>)
                -> tag_invoke_result_t<with_handle_t, Sender, Handle>
        {
            return tag_invoke(*this, std::forward<Sender>(sender), std::forward<Handle>(handle));
        }

        template <typename Handle>
        auto operator()(Handle &&handle) const
        {
            return pipable_from_right(*this, std::forward<Handle>(handle));
        }
    };

    inline constexpr with_handle_t with_handle;

}
}