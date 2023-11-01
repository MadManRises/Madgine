#pragma once

namespace Engine {
namespace Execution {

    struct Lifetime {

        ~Lifetime()
        {
            assert(mCount == 0);
        }

        template <typename Sender>
        void attach(Sender &&sender)
        {
            if (!mStopSource.stop_requested()) {
                (new attach_state<Sender> { std::forward<Sender>(sender), this })->start();
            }
        }
        auto end()
        {
            return end_sender { this };
        }
        void reset()
        {
            bool result = mStopSource.request_stop();
            assert(result);
            mStopSource = {};
        }

    private:
        void endImpl(VirtualReceiverBase<GenericResult> *receiver)
        {
            assert(!mReceiver);
            mReceiver = receiver;
            if (mCount.fetch_add(1) > 0) {
                bool result = mStopSource.request_stop();
                assert(result);
            }
            if (mCount.fetch_sub(1) == 1) {
                mReceiver->set_value();
                mReceiver = nullptr;
            }
        }

        void increaseCount()
        {
            ++mCount;
        }

        void decreaseCount()
        {
            if (mCount.fetch_sub(1) == 1) {
                if (mStopSource.stop_requested()) {
                    mReceiver->set_value();
                    mReceiver = nullptr;
                }
            }
        }

        template <typename Sender>
        struct attach_state;

        template <typename Sender>
        struct attach_receiver : execution_receiver<> {

            template <typename... V>
            void set_value(V &&...)
            {
                mState->mLifetime->decreaseCount();
                delete mState;
            }
            void set_done()
            {
                mState->mLifetime->decreaseCount();
                delete mState;
            }
            template <typename... R>
            void set_error(R &&...)
            {
                mState->mLifetime->decreaseCount();
                delete mState;
            }

            friend auto tag_invoke(get_stop_token_t, attach_receiver<Sender> &rec)
            {
                return rec.mToken;
            }

            attach_state<Sender> *mState;
            std::stop_token mToken;
        };

        template <typename Sender>
        struct attach_state {
            attach_state(Sender &&sender, Lifetime *lifetime)
                : mState(connect(std::forward<Sender>(sender), attach_receiver<Sender> { {}, this, lifetime->mStopSource.get_token() }))
                , mLifetime(lifetime)
            {
            }
            void start()
            {
                mLifetime->increaseCount();
                mState.start();
            }

            connect_result_t<Sender, attach_receiver<Sender>> mState;
            Lifetime *mLifetime;
        };

        struct end_state : VirtualReceiverBase<GenericResult> {
            end_state(Lifetime *lifetime)
                : mLifetime(lifetime)
            {
            }

            void start()
            {
                mLifetime->endImpl(this);
            }

            Lifetime *mLifetime;
        };
        struct end_sender {
            using is_sender = void;

            using result_type = GenericResult;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            using is_sender = void;

            template <typename Rec>
            friend auto tag_invoke(connect_t, end_sender &&sender, Rec &&rec)
            {
                return Execution::make_virtual_state<end_state, GenericResult>(std::forward<Rec>(rec), sender.mLifetime);
            }

            Lifetime *mLifetime;
        };

        std::stop_source mStopSource;
        std::atomic<uint32_t> mCount = 0;
        VirtualReceiverBase<GenericResult> *mReceiver = nullptr;
    };

}
}