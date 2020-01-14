#pragma once

namespace Engine {

namespace CLI {

    struct MODULES_EXPORT ParameterBase {

        ParameterBase(size_t minArgumentCount, size_t maxArgumentCount, std::vector<const char *> options, const char *help = nullptr);

        virtual bool parse(const std::vector<const char *> &args) = 0;
        virtual const char *typeName() = 0;

        void init();
        std::string help();
        const std::vector<const char *> &options();

    private:
        std::vector<const char *> mOptions;
        const char *mHelp = nullptr;
        size_t mMinArgumentCount, mMaxArgumentCount;

        bool mInitialized = false;
    };

}
}