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

    template <typename T>
    struct ParameterImpl : ParameterBase {

        ParameterImpl(std::vector<const char *> options, T defaultValue = {}, const char *help = nullptr)
            : ParameterBase(std::is_same_v<T, bool> ? 0 : 1, 1, std::move(options), help)
            , mValue(std::move(defaultValue))
        {
        }

        bool parse(const std::vector<const char *> &args) override;

        const char *typeName() override
        {
            return std::is_same_v<T, std::string> ? "string" : typeid(T).name();
        }

        const T &operator*()
        {
            init();
            return mValue;
        }

        const T *operator->()
        {
            init();
            return &mValue;
        }

        operator const T &()
        {
            init();
            return mValue;
        }

    private:
        T mValue;
    };

	template <>
    inline bool ParameterImpl<bool>::parse(const std::vector<const char *> &args)
    {
        if (args.empty())
            mValue = true;
        else {
            std::string_view arg = args.front();
            if (arg == "on" || arg == "true")
                mValue = true;
            else if (arg == "off" || arg == "false")
                mValue = false;
            else
                return false;
        }
        return true;
    }

    template <>
    inline bool ParameterImpl<int>::parse(const std::vector<const char *> &args)
    {
        errno = 0;
        char *end;
        int value = strtol(args.front(), &end, 0);
        if (errno == 0 && *end == '\0') {
            mValue = value;
            return true;
        } else {
            return false;
        }
    }

    template <>
    inline bool ParameterImpl<std::string>::parse(const std::vector<const char *> &args)
    {
        mValue = args.front();
        return true;
    }

    template <typename T>
    struct Parameter : ParameterImpl<T> {
        Parameter(std::vector<const char *> options, T defaultValue, const char *help)
            : ParameterImpl<T>(std::move(options), std::move(defaultValue), help)
        {
        }
    };

    template <>
    struct Parameter<std::string> : ParameterImpl<std::string> {
        //using ParameterImpl<std::string>::ParameterImpl;
        Parameter(std::vector<const char *> options, std::string defaultValue, const char *help)
            : ParameterImpl(std::move(options), std::move(defaultValue), help)
        {
        }

        operator const char *()
        {
            return static_cast<const std::string &>(*this).c_str();
        }
    };

    struct MODULES_EXPORT CLICore {
        CLICore(int argc, char **argv);
        ~CLICore();

        std::string help();

        static const CLICore &getSingleton();
        static std::vector<ParameterBase *> &parameters();

        std::map<std::string_view, std::vector<const char *>> mArguments;
    };
}
}