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

    mIncludes[level][mConditionalsBitMask].insert(std::string { file });
}

void CppFile::beginNamespace(std::string_view ns)
{
    mNamespaceStack.push(&std::get<Namespace>(mNamespaceStack.top()->mElements.emplace_back(Namespace { std::string { ns }, mConditionalsBitMask })));
}

void CppFile::endNamespace()
{
    assert(mNamespaceStack.size() > 1);
    mNamespaceStack.pop();
}

CppFile::CustomCodeBuilder::~CustomCodeBuilder()
{
    mFile->addCustomCodeBlock(mStream.str());
}

CppFile::CustomCodeBuilder &CppFile::CustomCodeBuilder::operator<<(std::string_view code)
{
    mStream << code;
    return *this;
}

CppFile::CustomCodeBuilder CppFile::operator<<(std::string_view code)
{
    CustomCodeBuilder builder { this };
    builder.mStream << code;
    return builder;
}

void CppFile::addCustomCodeBlock(std::string_view code)
{
    mNamespaceStack.top()->mElements.emplace_back(CustomCodeBlock {
        std::string { code },
        mConditionalsBitMask
    });
}

void CppFile::statement(Statement statement)
{
    throw 0;
}

void CppFile::generate(std::ostream &stream)
{
    assert(mNamespaceStack.size() == 1);
    assert(mConditionalsBitMask == Engine::BitArray<64> {});

    for (std::map<Engine::BitArray<64>, std::set<std::string>> &includeMap : mIncludes) {
        for (std::pair<const Engine::BitArray<64>, std::set<std::string>> &p : includeMap) {
            stream << generateIncludeGuard(p.first);

            for (const std::string &file : p.second) {
                stream << "#include \"" << file << "\"\n";
            }

            if (p.first != Engine::BitArray<64> {})
                stream << "#endif\n";
        }
        stream << "\n";
    }

    stream << "\n";

    generate(stream, mGlobalNamespace);
}

void CppFile::generate(std::ostream &stream, CustomCodeBlock &block)
{
    stream << generateIncludeGuard(block.mConditionals);

    stream << block.mCode;

    if (block.mConditionals != Engine::BitArray<64> {})
        stream << "#endif\n";
}

void CppFile::generate(std::ostream &stream, Namespace &ns)
{
    stream << generateIncludeGuard(ns.mConditionals);

    if (!ns.mName.empty())
        stream << "namespace " << ns.mName << "{\n";

    for (Statement &s : ns.mElements) {
        std::visit([&](auto &block) { generate(stream, block); }, s);
    }

    if (!ns.mName.empty())
        stream << "}\n";

    if (ns.mConditionals != Engine::BitArray<64> {})
        stream << "#endif\n";
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

std::string CppFile::generateIncludeGuard(const Engine::BitArray<64> &conditionals)
{
    if (conditionals == Engine::BitArray<64> {})
        return "";

    std::string result = "#if ";

    bool first = true;

    for (size_t i = 0; i < 64; ++i) {
        if (conditionals[i]) {
            if (first)
                first = false;
            else
                result += " && ";
            result += "defined(" + mConditionalTokenList[i] + ")";
        }
    }

    return result + "\n";
}

}