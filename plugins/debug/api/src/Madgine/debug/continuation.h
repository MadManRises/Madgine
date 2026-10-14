#pragma once

#include "Generic/typed_ptr.h"

#include "contextinfo.h"
#include "debuglocation.h"

namespace Engine {
namespace Debug {

    struct get_continuation_t {

        using signature = Debug::Continuation *();

        template <typename T>
            requires(!tag_invocable<get_continuation_t, T &>)
        auto operator()(T &t) const
        {
            return nullptr;
        }

        template <typename T>
            requires tag_invocable<get_continuation_t, T &>
        auto operator()(T &t) const
            noexcept(is_nothrow_tag_invocable_v<get_continuation_t, T &>)
                -> tag_invoke_result_t<get_continuation_t, T &>
        {
            return tag_invoke(*this, t);
        }
    };

    inline constexpr get_continuation_t get_continuation;

    enum class ContinuationMode {
        Continue,
        Step,
        StepInto,
        StopRequested,
        Done
    };

    enum class ContinuationType {
        Flow,
        Return,
        Error,
        Done,
        Cancelling
    };

    struct Continuation {
    private:
        struct Base {
            virtual ~Base() = default;

            virtual void call(ContinuationMode triggerMode) = 0;

            virtual void visitArguments(std::ostream &) = 0;
        };

        template <typename F, typename... Args>
        struct Impl : Base {
            Impl(F &&callback, Args &&...args)
                : mCallback(std::forward<F>(callback))
                , mArgs { std::forward<Args>(args)... }
            {
            }

            virtual void call(ContinuationMode triggerMode) override
            {
                TupleUnpacker::invokeExpand(std::forward<F>(mCallback), triggerMode, std::move(mArgs));
            }

            virtual void visitArguments(std::ostream &out) override
            {
                StringUtil::StreamJoiner joiner { out, "\n" };
                TupleUnpacker::forEach(mArgs, [&](auto &v) {
                    if constexpr (requires { out << v; }) {
                        joiner.next() << v;
                    } else {
                        joiner.next() << typeid(v).name();
                    }
                });
            }

            F mCallback;
            std::tuple<Args...> mArgs;
        };

        static constexpr uintptr_t sModeMask = 0x7;

        template <typename Rec, typename F, typename... Args>
        void finalPass(TypedPtr location, Rec &rec, F &&callback, ContinuationType type, IndexType<size_t> line = {}, Args &&...args)
        {
            auto f = [this, callback { forward_capture<F>(callback) }](ContinuationMode triggerMode, Rec &rec, Args &&...args) mutable {
                assert(mode() == ContinuationMode::Done);
                if (triggerMode == Debug::ContinuationMode::Step || triggerMode == Debug::ContinuationMode::StepInto) {
                    Debug::Continuation *parent = Debug::get_continuation(rec);
                    if (parent) {
                        parent->setMode(triggerMode);
                    }
                }
                std::forward<F>(callback)(rec, std::forward<Args>(args)...);
            };

            uintptr_t prev = mImpl.load();
            assert(!toImpl(prev));

            if ((line && toMode(prev) != ContinuationMode::Continue) || toMode(prev) == ContinuationMode::StopRequested || get_debug_context(rec).wantsPause(location, type, line)) {
                suspend(location, rec, std::move(f), type, std::forward<Args>(args)...);
                return;
            }

            while (!mImpl.compare_exchange_weak(prev, static_cast<uintptr_t>(ContinuationMode::Done))) {
                assert(!toImpl(prev));

                if ((line && toMode(prev) != ContinuationMode::Continue) || toMode(prev) == ContinuationMode::StopRequested) {
                    suspend(location, rec, std::move(f), type, std::forward<Args>(args)...);
                    return;
                }
            }

            f(ContinuationMode::Continue, rec, std::forward<Args>(args)...);
        }

    public:
        Continuation() = default;
        Continuation(const Continuation &) = delete;
        ~Continuation()
        {
            assert(!implPtr());
            assert(mode() == ContinuationMode::Done);
        }

        
        void start(auto &rec) {

            Continuation *parent = get_continuation(rec);

            ContinuationMode target = ContinuationMode::Continue;
            
            if (parent && parent->mode() == Debug::ContinuationMode::StepInto)
                target = Debug::ContinuationMode::Step;

            uintptr_t prev = mImpl.exchange(static_cast<uintptr_t>(target));

            assert(toMode(prev) == ContinuationMode::Done);
        }

        template <typename Rec, typename F, typename... Args>
        void suspend(TypedPtr location, Rec &rec, F &&callback, ContinuationType type, Args &&...args)
        {
            auto f = [&rec, callback { forward_capture<F>(callback) }](ContinuationMode triggerMode, Args &&...args) mutable {
                if (triggerMode == Debug::ContinuationMode::Done) {
                    rec.set_done();
                } else if constexpr (std::invocable<F, ContinuationMode, Rec &, Args...>) { // :(
                    std::forward<F>(callback)(triggerMode, rec, std::forward<Args>(args)...);
                } else {
                    std::forward<F>(callback)(rec, std::forward<Args>(args)...);
                }
            };

            Base *impl = new Impl<decltype(f), Args...>(std::move(f), std::forward<Args>(args)...);

            ContextInfo &debugContext = get_debug_context(rec);

            uintptr_t prev = mImpl.load();
            for (;;) {
                ContinuationMode prevMode = toMode(prev);
                if (prevMode == ContinuationMode::Done) {
                    impl->call(ContinuationMode::Done);
                    delete impl;
                    return;
                }

                ContinuationType stored = (prevMode == ContinuationMode::StopRequested) ? ContinuationType::Cancelling : type;
                if (mImpl.compare_exchange_weak(prev, reinterpret_cast<uintptr_t>(impl) | static_cast<uintptr_t>(stored)))
                    break;
            }

            debugContext.suspend(location, type);
        }

        template <typename Rec, typename F, typename... Args>
        void pass(TypedPtr location, Rec &rec, F &&callback, IndexType<size_t> line = {}, Args &&...args)
        {
            ContinuationMode currentMode = mode();

            if ((line && currentMode != ContinuationMode::Continue) || get_debug_context(rec).wantsPause(location, ContinuationType::Flow, line)) {
                suspend(location, rec, std::forward<F>(callback), ContinuationType::Flow, std::forward<Args>(args)...);
            } else {
                std::forward<F>(callback)(rec, std::forward<Args>(args)...);
            }
        }

        template <typename Rec, typename... Args>
        void returnError(TypedPtr location, Rec &rec, IndexType<size_t> line = {}, Args &&...args)
        {
            finalPass(location, rec, [](Rec &rec, Args &&...result) { rec.set_error(std::forward<Args>(result)...); }, ContinuationType::Error, line, std::forward<Args>(args)...);
        }

        template <typename Rec, typename... Args>
        void returnValue(TypedPtr location, Rec &rec, IndexType<size_t> line = {}, Args &&...args)
        {
            finalPass(location, rec, [](Rec &rec, Args &&...value) { rec.set_value(std::forward<Args>(value)...); }, ContinuationType::Return, line, std::forward<Args>(args)...);
        }

        template <typename Rec>
        void returnDone(TypedPtr location, Rec &rec, IndexType<size_t> line = {})
        {
            finalPass(location, rec, [](Rec &rec) { rec.set_done(); }, ContinuationType::Done, line);
        }

        explicit operator bool() const
        {
            Base *impl = implPtr();
            return impl;
        }

        bool stop()
        {
            uintptr_t prev = mImpl.load();
            for (;;) {
                if (Base *impl = toImpl(prev)) {
                    if (toType(prev) == ContinuationType::Cancelling)
                        return false;
                    if (mImpl.compare_exchange_weak(prev, reinterpret_cast<uintptr_t>(impl) | static_cast<uintptr_t>(ContinuationType::Cancelling)))
                        return true;
                } else {
                    ContinuationMode mode = toMode(prev);
                    if (mode == ContinuationMode::StopRequested || mode == ContinuationMode::Done)
                        return false;
                    if (mImpl.compare_exchange_weak(prev, static_cast<uintptr_t>(ContinuationMode::StopRequested)))
                        return true;
                }
            }
        }

        void done()
        {
            uintptr_t impl = mImpl.exchange(static_cast<uintptr_t>(ContinuationMode::Done));
            Base *ptr = toImpl(impl);
            if (ptr) {
                assert(toType(impl) == ContinuationType::Cancelling);
                ptr->call(ContinuationMode::Done);
                delete ptr;
            } else {
                assert(toMode(impl) == ContinuationMode::StopRequested);
            }
        }

        ContinuationType type() const
        {
            return toType(mImpl.load());
        }

        void setMode(ContinuationMode mode)
        {
            uintptr_t prev = mImpl.load();
            do {
                assert(!toImpl(prev));
                ContinuationMode cur = toMode(prev);
                if (cur == ContinuationMode::StopRequested || cur == ContinuationMode::Done)
                    return;
            } while (!mImpl.compare_exchange_weak(prev, static_cast<uintptr_t>(mode)));
        }

        ContinuationMode mode() const
        {
            return toMode(mImpl.load());
        }

        void operator()(ContinuationMode newMode)
        {
            assert(newMode != ContinuationMode::StopRequested);

            if (newMode == ContinuationMode::Done) {
                done();
                return;
            }

            uintptr_t prev = mImpl.load();
            uintptr_t target;
            do {
                if (toImpl(prev)) {
                    ContinuationType type = toType(prev);
                    if (type == ContinuationType::Cancelling)
                        return;
                    target = (type == ContinuationType::Flow) ? static_cast<uintptr_t>(newMode) : static_cast<uintptr_t>(ContinuationMode::Done);
                } else {
                    ContinuationMode mode = toMode(prev);
                    if (mode == ContinuationMode::StopRequested || mode == ContinuationMode::Done)
                        return;
                    target = static_cast<uintptr_t>(newMode);
                }
            } while (!mImpl.compare_exchange_weak(prev, target));

            Base *impl = toImpl(prev);
            if (impl) {
                impl->call(newMode);
                delete impl;
            }
        }

        void pause()
        {
            uintptr_t prev = mImpl.load();
            do {
                if (toImpl(prev)) {
                    return;
                }
                ContinuationMode mode = toMode(prev);
                if (mode == ContinuationMode::StopRequested || mode == ContinuationMode::Done) {
                    return;
                }
            } while (!mImpl.compare_exchange_weak(prev, static_cast<uintptr_t>(ContinuationMode::StepInto)));
        }

        void visitArguments(std::ostream &out) const
        {
            Base *impl = implPtr();
            assert(impl);
            impl->visitArguments(out);
        }

    protected:
        static Base *toImpl(uintptr_t p)
        {
            return reinterpret_cast<Base *>(p & ~sModeMask);
        }

        Base *implPtr() const
        {
            return toImpl(mImpl.load());
        }

        static ContinuationMode toMode(uintptr_t p)
        {
            assert(!toImpl(p));
            return static_cast<ContinuationMode>(p & sModeMask);
        }

        static ContinuationType toType(uintptr_t p)
        {
            assert(toImpl(p));
            return static_cast<ContinuationType>(p & sModeMask);
        }

    private:
        std::atomic<uintptr_t> mImpl = static_cast<uintptr_t>(ContinuationMode::Done);
    };

}
}