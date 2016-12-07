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
    void addStatement(Ogre::unique_ptr<const Statements::Statement> &&s);

    const std::list<std::string> &arguments() const;

	void clear();

	virtual size_t calculateSize() const override final;
protected:
	virtual void loadImpl() override final;
	virtual void unloadImpl() override final;

private:
    std::list<Ogre::unique_ptr<const Statements::Statement>> mStatements;
    std::list<std::string> mArguments;
};


}
}
}


