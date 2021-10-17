#pragma once


namespace Engine {

    struct Guard {
        Guard(std::function<void()> &&init, std::function<void()> &&finalize = {})
            : mFinalize(std::move(finalize))
        {
            if (init)
                init();
        }

        ~Guard()
        {
            if (mFinalize)
                mFinalize();
        }

    private:
        std::function<void()> mFinalize;
    };

}