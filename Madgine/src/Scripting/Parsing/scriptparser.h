#pragma once

#include "Scripting\Types\struct.h"
#include "entitynodeptr.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class MADGINE_EXPORT ScriptParser : public Ogre::ResourceManager,
    public Ogre::Singleton<ScriptParser> {
public:
    ScriptParser();
    ~ScriptParser();

    const Ogre::StringVector &getScriptPatterns() const;
    float getLoadingOrder() const;

    virtual void parseScript(Ogre::DataStreamPtr &stream, const Ogre::String &group) override;

    const MethodNodePtr &getGlobalMethod(const std::string &name);

    const EntityNodePtr &getEntityDescription(const std::string &name);

	Struct &getPrototype(const std::string &name);

    bool hasGlobalMethod(const std::string &name);

	void reparseFile(const Ogre::String &name, const Ogre::String &group);

protected:
	

	virtual Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
		const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
		const Ogre::NameValuePairList *createParams) final;

private:
    enum TokenType {
        NameToken,
        CurlyBracketOpen,
        CurlyBracketClose,
        BracketOpen,
        BracketClose,
        VectorBracketOpen,
        VectorBracketClose,
		ColonToken,
        CommaToken,
        SOFToken,
        EOFToken,
        NewlineToken,
        StringToken,
        IntToken,
        BoolToken,
        FloatToken,
        DotToken,
        AssignmentToken,
        BinaryLogical,
        LogicalNot,
        IfToken,
        WhileToken,
        ForToken,
        ElseToken,
        MinusToken,
        PlusToken,
		DivideToken,
        ReturnToken,
        ComparatorToken,
        SemikolonToken,
        NullToken
    };

	void parseScript(Ogre::DataStreamPtr &stream, const Ogre::String &group, bool reload);

    static std::string typeToString(TokenType type);

    void doRead();
    void skipNewlines();

    char safeRead();

    std::string consume();
    std::string consume(TokenType expected);


    void parseEntity();
	void parsePrototype();
    void parseMethod(const EntityNodePtr &entity = EntityNodePtr());
    Ogre::unique_ptr<const Statements::Statement> parseFullStatement();
    Ogre::unique_ptr<const Statements::Statement> parseSingleStatement();
	Ogre::unique_ptr<const Statements::Statement> parseTerm();
	ValueType parseConst();

#define MAKE_STATEMENT(T) OGRE_MAKE_UNIQUE(T)

private:

    Ogre::DataStreamPtr mStream;

    size_t mLineBase;

    float mLoadOrder;
    Ogre::StringVector mScriptPatterns;

    std::map<std::string, EntityNodePtr> mGlobalTypes;
    std::map<std::string, MethodNodePtr> mGlobalMethods;
	std::map<std::string, ScopeRef<Struct>> mPrototypes;

    Ogre::ResourceGroupManager *mRsgm;

    TokenType mNextToken;
    std::string mNextText;
    size_t mCurrentLine;
	std::string mFile, mGroup;
	bool mReload;
};

}
}
}


