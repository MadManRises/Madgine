#pragma once

namespace Engine {

namespace CLI {

    struct CLI_EXPORT CLICore {
        CLICore(int argc, char **argv);
        ~CLICore();

        std::string help();

		static bool isInitialized();
        static const CLICore &getSingleton();
        static std::vector<ParameterBase *> &parameters();

        std::map<std::string_view, std::vector<const char *>> mArguments;
    };
}
}