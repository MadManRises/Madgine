#pragma once

#include "Scripting/Statements/statement.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class MethodNode : public TextResource {
public:
	using TextResource::TextResource;
	MethodNode(const MethodNode &) = delete;

    ValueType run(Scope *scope, Stack &stack) const;
    void addArgument(const std::string &arg);
    void addStatement(std::unique_ptr<const Statements::Statement> &&s);

    const std::list<std::string> &arguments() const;

	void clear();

private:
    std::list<std::unique_ptr<const Statements::Statement>> mStatements;
    std::list<std::string> mArguments;
};

}
}
}


