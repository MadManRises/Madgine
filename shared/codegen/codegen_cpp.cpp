#include "codegenlib.h"

#include "codegen_cpp.h"

namespace CodeGen {

CppFile::CppFile()
    : mNamespaceStack { { &mGlobalNamespace } }
{
}

void CppFile::include(size_t level, const std::string_view &file)
{
    if (mIncludes.size() <= level)
        mIncludes.resize(level + 1);

    mIncludes[level][mConditionalsBitMask].insert(std::string { file });
}

void CppFile::beginNamespace(const std::string_view &ns)
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

CppFile::CustomCodeBuilder &CppFile::CustomCodeBuilder::operator<<(const std::string_view &code)
{
    mStream << code;
    return *this;
}

CppFile::CustomCodeBuilder CppFile::operator<<(const std::string_view &code)
{
    CustomCodeBuilder builder { this };
    builder.mStream << code;
    return builder;
}

void CppFile::addCustomCodeBlock(const std::string_view &code)
{
    mNamespaceStack.top()->mElements.emplace_back(CustomCodeBlock {
        mConditionalsBitMask,
        std::string { code },
    });
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

    mGlobalNamespace.generate(stream, *this);
}

void CppFile::CustomCodeBlock::generate(std::ostream &stream, CppFile &file)
{
    stream << file.generateIncludeGuard(mConditionals);

    stream << mCode;

    if (mConditionals != Engine::BitArray<64> {})
        stream << "#endif\n";
}

void CppFile::Namespace::generate(std::ostream &stream, CppFile &file)
{
    stream << file.generateIncludeGuard(mConditionals);

    if (!mName.empty())
        stream << "namespace " << mName << "{\n";

    for (std::variant<Namespace, CustomCodeBlock> &v : mElements) {
        std::visit([&](auto &block) { block.generate(stream, file); }, v);
    }

    if (!mName.empty())
        stream << "}\n";

    if (mConditionals != Engine::BitArray<64> {})
        stream << "#endif\n";
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