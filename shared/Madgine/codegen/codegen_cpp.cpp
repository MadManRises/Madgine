#include "codegenlib.h"

#include "codegen_cpp.h"

#include "statement.h"

namespace CodeGen {

CppFile::CppFile()
    : mNamespaceStack { { &mGlobalNamespace } }
{
}

void CppFile::include(size_t level, std::string_view file)
{
    if (mIncludes.size() <= level)
        mIncludes.resize(level + 1);

    mIncludes[level][{ mConditionalsBitMask }].insert(std::string { file });
}

void CppFile::beginNamespace(std::string_view ns)
{
    mNamespaceStack.push(&std::get<Namespace>(mNamespaceStack.top()->mElements.emplace_back(Namespace {
        { { mConditionalsBitMask } },
        std::string { ns } })));
}

void CppFile::endNamespace()
{
    assert(mNamespaceStack.size() > 1);
    mNamespaceStack.pop();
}

void CppFile::statement(Statement statement)
{
    mNamespaceStack.top()->mElements.emplace_back(std::move(statement));
}

void CppFile::generate(std::ostream &stream)
{
    assert(mNamespaceStack.size() == 1);
    assert(mConditionalsBitMask == Engine::BitArray<62> {});

    for (std::map<Engine::BitArray<62>, std::set<std::string>> &includeMap : mIncludes) {
        for (std::pair<const Engine::BitArray<62>, std::set<std::string>> &p : includeMap) {
            bool hasGuard = openCStyleGuard(stream, { p.first });

            for (const std::string &file : p.second) {
                stream << "#include \"" << file << "\"\n";
            }

            if (hasGuard)
                stream << "#endif\n";
        }
        stream << "\n";
    }

    stream << "\n";

    generate(stream, mGlobalNamespace);
}

void CppFile::generate(std::ostream &stream, CustomCodeBlock &block)
{
    stream << block.mCode;
}

void CppFile::generate(std::ostream &stream, Namespace &ns)
{
    if (!ns.mName.empty())
        stream << "namespace " << ns.mName << "{\n";

    for (Statement &s : ns.mElements) {
        generate(stream, s);
    }

    if (!ns.mName.empty())
        stream << "}\n";
}

void CppFile::generate(std::ostream &stream, VariableRead &read)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, VariableDefinition &def)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, MemberAccess &access)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Return &ret)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Assignment &assign)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, ArithOperation &op)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Constructor &con)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Constant &c)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Comment &c)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream, Statement &s)
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