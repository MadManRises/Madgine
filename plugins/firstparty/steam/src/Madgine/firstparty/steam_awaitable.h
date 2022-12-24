#pragma once

namespace Engine {
namespace FirstParty {

    template <typename R>
    struct Steam_Awaitable {

        Steam_Awaitable(SteamAPICall_t call)
            : mCall(std::move(call))
        {
        }

        void callback(R* result, bool bIOFailure) {
            mResult = result;
            mHandle();
        }

        bool await_ready() {
            return false;
        }

        void await_suspend(Threading::TaskHandle handle) {
            mHandle = std::move(handle);
            mCallResult.Set(mCall, this, &Steam_Awaitable::callback);
        }

        R* await_resume() {
            return mResult;
        }

        SteamAPICall_t mCall;
        CCallResult<Steam_Awaitable, R> mCallResult;
        Threading::TaskHandle mHandle;
        R *mResult;
    };

}
}