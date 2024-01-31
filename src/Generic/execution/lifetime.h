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
        void end()
        {
            if (mStopSource.request_stop())
                decreaseCount();
        }
        auto ended()
        {
            return ended_sender { this };
        }
        void reset()
        {
            if (!mStopSource.request_stop())
                increaseCount();
            mStopSource = {};
        }

    private:
        void endedImpl(VirtualReceiverBase<GenericResult> *receiver)
        {
            increaseCount();
            assert(!mReceiver);
            mReceiver = receiver;
            decreaseCount();
        }

        void increaseCount()
        {
            ++mCount;
        }

        void decreaseCount()
        {
            if (mCount.fetch_sub(1) == 1) {
                assert(mStopSource.stop_requested());
                if (mReceiver) {
                    mReceiver->set_value();
                    mReceiver = nullptr;
                }
            }
        }

        template <typename Sender>
        struct attach_state;

        template <typename Sender>
        struct attach_receiver : execution_receiver<> {

            void set_value() {
                mState->mLifetime->decreaseCount();
                delete mState;
            }

            template <typename... V>
            [[nodiscard]] std::monostate set_value(V &&...)
            {
                mState->mLifetime->decreaseCount();
                delete mState;
                return {};
            }
            void set_done()
            {
                mState->mLifetime->decreaseCount();
                delete mState;
            }
            template <typename... R>
            [[nodiscard]] std::monostate set_error(R &&...)
            {
                mState->mLifetime->decreaseCount();
                delete mState;
                return {};
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

        template <typename Rec>
        struct ended_state : VirtualReceiverBase<GenericResult> {
            ended_state(Rec &&rec, Lifetime *lifetime)
                : mRec(std::forward<Rec>(rec))
                , mCallback(get_stop_token(mRec), callback { lifetime })
                , mLifetime(lifetime)
            {
            }

            void start()
            {
                mLifetime->endedImpl(this);
            }

            virtual void set_value() override {
                mRec.set_value();
            }

            virtual void set_error(GenericResult result) override {
                mRec.set_error(result);
            }

            virtual void set_done() override {
                mRec.set_done();
            }

            struct callback {
                void operator()() {
                    mLifetime->end();
                }

                Lifetime *mLifetime;
            };

            Rec mRec;
            std::stop_callback<callback> mCallback;
            Lifetime *mLifetime;
        };
        struct ended_sender {
            using is_sender = void;

            using result_type = GenericResult;
            template <template <typename...> typename Tuple>
            using value_types = Tuple<>;

            using is_sender = void;

            template <typename Rec>
            friend auto tag_invoke(connect_t, ended_sender &&sender, Rec &&rec)
            {
                return ended_state<Rec>(std::forward<Rec>(rec), sender.mLifetime);
            }

            Lifetime *mLifetime;
        };

        std::stop_source mStopSource;
        std::atomic<uint32_t> mCount = 1;
        VirtualReceiverBase<GenericResult> *mReceiver = nullptr;
    };

}
}