#pragma once

#include "codegen.h"
#include "statement.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT CppFile : File {

    CppFile();
    CppFile(const CppFile &) = delete;

    CppFile &operator=(const CppFile &) = delete;

    void include(size_t level, std::string_view file);

    void beginNamespace(std::string_view ns);
    void endNamespace();

    virtual void statement(Statement statement) override;

    struct MADGINE_CODEGEN_EXPORT CustomCodeBuilder {
        CustomCodeBuilder(CppFile *file)
            : mFile(file)
        {
        }
        CustomCodeBuilder(CustomCodeBuilder &&) = default;
        ~CustomCodeBuilder();

        CustomCodeBuilder &operator<<(std::string_view code);
        template <typename T, typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
        CustomCodeBuilder &operator<<(const T &t)
        {
            return (*this) << std::to_string(t);
        }

        CppFile *mFile;
        std::stringstream mStream;
    };

    CustomCodeBuilder operator<<(std::string_view code);

    void addCustomCodeBlock(std::string_view code);

    void generate(std::ostream &stream);

    void generate(std::ostream &stream, Namespace &ns);
    void generate(std::ostream &stream, CustomCodeBlock &block);
    void generate(std::ostream &stream, VariableRead &read);
    void generate(std::ostream &stream, VariableDefinition &def);
    void generate(std::ostream &stream, MemberAccess &access);
    void generate(std::ostream &stream, Return &ret);
    void generate(std::ostream &stream, Assignment &assign);
    void generate(std::ostream &stream, ArithOperation &op);
    void generate(std::ostream &stream, Constructor &con);
    void generate(std::ostream &stream, Constant &c);

private:
    std::string generateIncludeGuard(const Engine::BitArray<64> &conditionals);

    std::vector<std::map<Engine::BitArray<64>, std::set<std::string>>> mIncludes;

    Namespace mGlobalNamespace;
    std::stack<Namespace *> mNamespaceStack;
};

}