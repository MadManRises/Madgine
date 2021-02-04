#pragma once

#include "codegen.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT CppFile : File {

    CppFile();

    void include(size_t level, const std::string_view &file);

    void beginNamespace(const std::string_view &ns);
    void endNamespace();

    struct MADGINE_CODEGEN_EXPORT CustomCodeBuilder {
        CustomCodeBuilder(CustomCodeBuilder &&) = default;
        ~CustomCodeBuilder();

        CustomCodeBuilder &operator<<(const std::string_view &code);
        template <typename T, typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
        CustomCodeBuilder &operator<<(const T &t)
        {
            return (*this) << std::to_string(t);
        }

        CppFile *mFile;
        std::stringstream mStream;
    };

    CustomCodeBuilder operator<<(const std::string_view &code);

    void addCustomCodeBlock(const std::string_view &code);

    void generate(std::ostream &stream);

private:
    std::string generateIncludeGuard(const Engine::BitArray<64> &conditionals);

    std::vector<std::map<Engine::BitArray<64>, std::set<std::string>>> mIncludes;

    struct CustomCodeBlock {
        Engine::BitArray<64> mConditionals;
        std::string mCode;

        void generate(std::ostream &stream, CppFile &file);
    };

    struct Namespace {
        std::string mName;
        Engine::BitArray<64> mConditionals;
        std::list<std::variant<Namespace, CustomCodeBlock>> mElements;

        void generate(std::ostream &stream, CppFile &file);
    };

    Namespace mGlobalNamespace;
    std::stack<Namespace *> mNamespaceStack;
};

}