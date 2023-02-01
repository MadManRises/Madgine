#pragma once

#include "Generic/sender.h"

namespace Engine {
namespace FirstParty {

    template <typename R>
    auto steam_sender(SteamAPICall_t call)
    {
        return make_sender<R>(
            [=]<typename Rec>(Rec &&rec) {
                struct state {
                    void start()
                    {
                        mCallResult.Set(mCall, this, &state::callback);
                    }
                    void callback(R *result, bool bIOFailure)
                    {
                        mRec.set_value(*result);
                    }
                    Rec mRec;
                    SteamAPICall_t mCall;
                    CCallResult<state, R> mCallResult;                    
                };
                return state { std::forward<Rec>(rec), call };
            });
    }

}
}