#include "../moduleslib.h"

#include "Interfaces/stringutil.h"
#include "cli.h"

namespace Engine {

CLIOption<bool> showHelp{ { "--help", "-h" }, false, "Show this help message" };

static CLI *sSingleton = nullptr;

CLI::CLI(int argc, char **argv)
{
    assert(!sSingleton);
    sSingleton = this;

    assert(argc > 0);
    char **it = argv + 1;
    char **end = argv + argc;

    if (it != end && **it != '-') {
        LOG_ERROR("Positional arguments are not supported!");
        ++it;
        while (it != end && **it != '-')
            ++it;
    }

    while (it != end) {
        assert(**it == '-');
        auto pib = mArguments.try_emplace(*it);
        if (!pib.second) {
            LOG_WARNING("Argument '" << *it << "' provided twice! Last occurence will be used!");
            pib.first->second.clear();
        }
        std::vector<const char *> &args = pib.first->second;
        ++it;
        while (it != end && **it != '-') {
            args.emplace_back(*it);
            ++it;
        }
    }

    for (CLIOptionBase *option : options())
        option->init();

    std::set<std::string_view> unusedArguments;
    //use kvKeys
    for (auto &p : mArguments)
        unusedArguments.insert(p.first);
    for (CLIOptionBase *option : options())
        for (const char *o : option->options())
            unusedArguments.erase(o);
    for (std::string_view unusedArgument : unusedArguments)
        LOG_WARNING("Unrecognized command-line argument '" << unusedArgument << "' provided!");

    if (showHelp)
        LOG(help());
}

std::string CLI::help()
{
    std::stringstream ss;
    ss << "Help:\n";
    for (CLIOptionBase *option : CLI::options()) {
        ss << option->help() << "\n";
    }
    return ss.str();
}

const CLI &CLI::getSingleton()
{
    assert(sSingleton);
    return *sSingleton;
}

std::vector<CLIOptionBase *> &CLI::options()
{
    static std::vector<CLIOptionBase *> dummy;
    return dummy;
}

CLIOptionBase::CLIOptionBase(size_t minArgumentCount, size_t maxArgumentCount, std::vector<const char *> options, const char *help)
    : mMinArgumentCount(minArgumentCount)
    , mMaxArgumentCount(maxArgumentCount)
    , mOptions(std::move(options))
    , mHelp(help)
{
    CLI::options().emplace_back(this);
}

void CLIOptionBase::init()
{
    if (!mInitialized) {
        mInitialized = true;

        const std::vector<const char *> *args = nullptr;
        const char *optionName = nullptr;

        for (const char *option : mOptions) {
            auto it = CLI::getSingleton().mArguments.find(option);
            if (it != CLI::getSingleton().mArguments.end()) {
                if (args)
                    LOG_WARNING("Different versions of argument '" << option << "' provided! Which version is used is undefined!");

                args = &it->second;
                optionName = option;
            }
        }

        if (args) {
            if (args->size() < mMinArgumentCount) {
                LOG_ERROR("Unsufficient amount of arguments provided for option '" << optionName << "'!");
                if (mHelp)
                    LOG_ERROR("\t" << mHelp);
                return;
            }
            if (args->size() > mMaxArgumentCount) {
                LOG_WARNING("Too many arguments provided for option '" << optionName << "'! Superfluous arguments will be discarded!");
            }
            if (!parse(*args)) {
                LOG_ERROR("Invalid parameters provided for option '" << optionName << "': " << StringUtil::join(*args, ", "));
                if (mHelp)
                    LOG_ERROR("\t" << mHelp);
            }
        }
    }
}

std::string CLIOptionBase::help()
{
    std::stringstream ss;
    ss << StringUtil::join(mOptions, ", ") << ": \t";
    if (mHelp)
        ss << mHelp;
    else
        ss << "<no help provided>";
    ss << " (" << typeName() << ")";
    return ss.str();
}

const std::vector<const char *> &CLIOptionBase::options()
{
    return mOptions;
}

template <>
bool CLIOptionImpl<bool>::parse(const std::vector<const char *> &args)
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
bool CLIOptionImpl<int>::parse(const std::vector<const char *> &args)
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
bool CLIOptionImpl<std::string>::parse(const std::vector<const char *> &args)
{
    mValue = args.front();
    return true;
}
}