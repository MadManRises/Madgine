#pragma once

#include "Scripting\Types\struct.h"
#include "prototype.h"
#include "entitynode.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT ScriptParser : /*public Ogre::ResourceManager,*/
    public Singleton<ScriptParser> {
public:
    ScriptParser();
	ScriptParser(const ScriptParser &) = delete;
    virtual ~ScriptParser();

	void operator=(const ScriptParser &) = delete;

    /*const Ogre::StringVector &getScriptPatterns() const;
    float getLoadingOrder() const;

    virtual void parseScript(Ogre::DataStreamPtr &stream, const Ogre::String &group) override;*/

	void parseScript(std::istream &stream, const std::string &name, bool reload);

    const MethodNode &getGlobalMethod(const std::string &name);

    const EntityNode &getEntityDescription(const std::string &name);

	Prototype &getPrototype(const std::string &name);

    bool hasGlobalMethod(const std::string &name);

	void reparseFile(const std::string &name);

protected:
	

	/*virtual Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
		const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
		const Ogre::NameValuePairList *createParams) final;
		*/
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

	

    static std::string typeToString(TokenType type);

    void doRead();
    void skipNewlines();

    char safeRead();

    std::string consume();
    std::string consume(TokenType expected);


    void parseEntity();
	void parsePrototype();
    void parseMethod(EntityNode *entity = 0);
    std::unique_ptr<const Statements::Statement> parseFullStatement();
    std::unique_ptr<const Statements::Statement> parseSingleStatement();
	std::unique_ptr<const Statements::Statement> parseTerm();
	ValueType parseConst();

#define MAKE_STATEMENT(T) std::make_unique<T>

private:

    std::istream *mStream;

    std::streampos mLineBase;

    /*float mLoadOrder;
    Ogre::StringVector mScriptPatterns;*/

    std::map<std::string, EntityNode> mGlobalTypes;
    std::map<std::string, MethodNode> mGlobalMethods;
	std::map<std::string, Prototype> mPrototypes;

    //Ogre::ResourceGroupManager *mRsgm;

    TokenType mNextToken;
    std::string mNextText;
    size_t mCurrentLine;
	std::string mFile, mGroup;
	bool mReload;

};

}
}
}


