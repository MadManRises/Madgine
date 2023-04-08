#pragma once

#pragma once

#include "codegen.h"
#include "statement.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT CMakeFile : File {

    CMakeFile();
    CMakeFile(const CMakeFile &) = delete;

    CMakeFile &operator=(const CMakeFile &) = delete;

    void setVariable(std::string_view name, std::string_view string);

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
    std::vector<Statement> mStatements;
};

}