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
    void generate(std::ostream &stream, Comment &c);

    void generate(std::ostream &stream, Statement &s);

private:
    std::vector<std::map<Engine::BitArray<62>, std::set<std::string>>> mIncludes;

    Namespace mGlobalNamespace;
    std::stack<Namespace *> mNamespaceStack;
};

}