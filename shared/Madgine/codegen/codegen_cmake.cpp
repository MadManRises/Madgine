#include "codegenlib.h"

#include "codegen_cmake.h"

#include "statement.h"

#include "Meta/keyvalue/valuetype.h"

#include "Meta/serialize/streams/formatter.h"

namespace CodeGen {

static std::array<std::ctype<char>::mask, 256> sBracket = Engine::Serialize::generateMask(~Engine::Serialize::ctype::space, { { '(', Engine::Serialize::ctype::space, ~0 }, { ')', Engine::Serialize::ctype::space, ~0 } });
static std::locale sLocaleBracket { std::locale {}, new Engine::Serialize::ctype { sBracket.data() } };

CMakeFile::CMakeFile()
{
}

void CMakeFile::setVariable(std::string_view name, std::string_view string)
{
    statement(Assignment { name, Constant { Engine::ValueType { string } } });
}

void CMakeFile::statement(Statement statement)
{
    mStatements.emplace_back(std::move(statement));
}

void CMakeFile::generate(std::ostream &stream)
{
    assert(mConditionalsBitMask == Engine::BitArray<62> {});

    for (Statement &statement : mStatements) {
        generate(stream, statement);
        stream << '\n';
    }
}

void CMakeFile::generate(std::ostream &stream, CustomCodeBlock &block)
{
    stream << block.mCode;
}

void CMakeFile::generate(std::ostream &stream, Namespace &ns)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, VariableRead &read)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, VariableDefinition &def)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, MemberAccess &access)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, Return &ret)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, Assignment &assign)
{
    stream << "set(" << assign.mVariableName << " ";
    generate(stream, *assign.mStatement);
    stream << ")";
}

void CMakeFile::generate(std::ostream &stream, ArithOperation &op)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, Constructor &con)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, Constant &c)
{
    stream << c.mValue.toShortString();
}

void CMakeFile::generate(std::ostream &stream, Comment &c)
{
    throw 0;
}

void CMakeFile::generate(std::ostream &stream, Statement &s)
{

    std::visit([&](auto &block) {
        bool hasGuard;
        if constexpr (hasConditionals<decltype(block)>)
            hasGuard = openCStyleGuard(stream, block.mConditionals);

        generate(stream, block);

        if constexpr (hasConditionals<decltype(block)>)
            closeCStyleGuard(stream, hasGuard);
    },
        s);
}

}