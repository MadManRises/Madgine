#include "madginelib.h"
#include "scriptparser.h"
#include "Scripting/scriptingexception.h"
#include "Scripting/Parsing/parseexception.h"

#include "Scripting/Parsing/entitynode.h"
#include "Scripting/Parsing/methodnode.h"

#include "Scripting/Statements/scopesetter.h"
#include "Scripting/Statements/assignment.h"
#include "Scripting/Statements/methodcall.h"
#include "Scripting/Statements/constvalue.h"
#include "Scripting/Statements/accessor.h"
#include "Scripting/Statements/return.h"
#include "Scripting/Statements/logicaloperator.h"
#include "Scripting/Statements/negate.h"
#include "Scripting/Statements/if.h"
#include "Scripting/Statements/while.h"
#include "Scripting/Statements/comparator.h"
#include "Scripting/Statements/for.h"
#include "Scripting\Statements\ArithmeticalOperator.h"

#include "Database/exceptionmessages.h"

#include "Util\UtilMethods.h"


namespace Engine {
namespace Scripting {
namespace Parsing {


ScriptParser::ScriptParser() : mRsgm(
        &Ogre::ResourceGroupManager::getSingleton())
{
	mResourceType = "Scripting";

    mLoadOrder = 100.0f;
    mScriptPatterns.push_back("*.script");
    mRsgm->_registerScriptLoader(this);
	mRsgm->_registerResourceManager(mResourceType, this);
}

ScriptParser::~ScriptParser()
{

	if (mRsgm->getSingletonPtr()) {
		mRsgm->_unregisterScriptLoader(this);
		mRsgm->_unregisterResourceManager(mResourceType);
	}
}

const Ogre::StringVector &ScriptParser::getScriptPatterns() const
{
    return mScriptPatterns;
}

float ScriptParser::getLoadingOrder() const
{
    return mLoadOrder;
}

void ScriptParser::parseScript(Ogre::DataStreamPtr &stream,
	const Ogre::String &group)
{
	parseScript(stream, group, false);
}

void ScriptParser::parseScript(Ogre::DataStreamPtr &stream,
                               const Ogre::String &group,
								bool reload)
{
    mStream = stream;
	mGroup = group;
	mReload = reload;
	mFile = stream->getName();
    mCurrentLine = 1;
    mLineBase = mStream->tell();

    mNextText.clear();
    mNextToken = SOFToken;

	try {

		doRead();
		skipNewlines();

		while (mNextToken != EOFToken) {

			if (mNextToken != NameToken) throw 0;
			if (mNextText == "entity") {
				parseEntity();
			}
			else if (mNextText == "data") {
				parsePrototype();
			}
			else {
				parseMethod();
			}

			skipNewlines();

		}

	}
	catch (ParseException &e) {		
		LOG_ERROR(std::string("Skipping File: " + mFile));
	}

}

const MethodNodePtr &ScriptParser::getGlobalMethod(const std::string &name)
{
    auto it = mGlobalMethods.find(name);
    if (it == mGlobalMethods.end()) {
        throw ScriptingException(Database::Exceptions::noGlobalMethod(name));
    }
    return it->second;
}

const EntityNodePtr &ScriptParser::getEntityDescription(const std::string &name)
{
    auto it = mGlobalTypes.find(name);
    if (it == mGlobalTypes.end()) {
        MADGINE_THROW(ScriptingException(Database::Exceptions::unknownEntityDescription(name)));
    }
    return it->second;
}

Prototype & ScriptParser::getPrototype(const std::string & name)
{
	auto it = mPrototypes.find(name);
	if (it == mPrototypes.end()) {
		MADGINE_THROW(ScriptingException(Database::Exceptions::unknownPrototype(name)));
	}
	return it->second;
}

bool ScriptParser::hasGlobalMethod(const std::string &name)
{
    return mGlobalMethods.find(name) != mGlobalMethods.end();
}

void ScriptParser::reparseFile(const Ogre::String & name, const Ogre::String &group)
{
	Ogre::DataStreamPtr stream =
		Ogre::ResourceGroupManager::getSingleton().openResource(
			name, "General", true);
	parseScript(stream, group, true);
}

Ogre::Resource * ScriptParser::createImpl(const Ogre::String & name, Ogre::ResourceHandle handle, const Ogre::String & group, bool isManual, Ogre::ManualResourceLoader * loader, const Ogre::NameValuePairList * createParams)
{
	if (createParams->at("Type") == "Entity") {
		return new EntityNode(this, name, handle, group, isManual, loader);
	}
	else if (createParams->at("Type") == "Method") {
		return new MethodNode(this, name, handle, group, isManual, loader);
	}
	else {
		throw 0;
	}
}


std::string ScriptParser::typeToString(ScriptParser::TokenType type)
{
    switch (type) {
    case NameToken:
        return "Name";
    case CurlyBracketOpen:
    case CurlyBracketClose:
    case BracketOpen:
    case BracketClose:
        return "Bracket";
    case EOFToken:
        return "EOF";
    case CommaToken:
        return "Comma";
    case StringToken:
        return "String";
    case NewlineToken:
        return "Newline";
    case DotToken:
        return "Dot";
    case AssignmentToken:
        return "Assignment";
    case IntToken:
        return "Int";
    case ComparatorToken:
        return "Comparator";
    default:
        return "Unknown";
    }
}

void ScriptParser::parseEntity()
{

    consume(NameToken);
    std::string name = consume(NameToken);

	EntityNodePtr node;

	bool created = false;

    auto it = mGlobalTypes.find(name);
    if (it != mGlobalTypes.end()) {
		if (!mReload)
			throw ScriptingException(Database::Exceptions::doubleTypeDefinition(name));

		node = it->second;
		for (const std::pair<const std::string, MethodNodePtr> &method : node->getMethods()) {
			remove(method.second->getHandle());
		}
		node->clear();
	}
	else {
		Ogre::NameValuePairList param;
		param["Type"] = "Entity";

		node = mGlobalTypes[name] = createResource(name, mGroup, false, 0, &param).staticCast<EntityNode>();		
		created = true;
	}

	node->_notifyOrigin(mFile);
	node->setLineNr(mCurrentLine);

	try {

		if (mNextToken == ColonToken) {
			doRead();
			node->setPrototype(consume(NameToken));
		}

		skipNewlines();
		consume(CurlyBracketOpen);
		skipNewlines();

		while (mNextToken != CurlyBracketClose) {
			parseMethod(node);
			skipNewlines();
		}
		doRead();

	}
	catch (ParseException &e) {
		std::list<Util::TraceBack> traceback;
		traceback.emplace_back(mFile, mCurrentLine, name);
		Util::UtilMethods::log(e.what(), Ogre::LML_CRITICAL, traceback);
		if (created) {
			mGlobalTypes.erase(name);			
			remove(node->getHandle());
		}
		throw;
	}
}

void ScriptParser::parsePrototype()
{

	consume(NameToken);
	std::string name = consume(NameToken);

	try {
		auto it = mPrototypes.find(name);
		if (it != mPrototypes.end() && !mReload) {
			throw ParseException(Database::Exceptions::doubleTypeDefinition(name));
		}
		Prototype *node = &mPrototypes[name];
		node->clear();
		node->clearPrototype();

		if (mNextToken == ColonToken) {
			doRead();
			node->findPrototype(consume(NameToken));
		}

		skipNewlines();
		consume(CurlyBracketOpen);
		skipNewlines();

		while (mNextToken != CurlyBracketClose) {
			std::string fieldName = consume(NameToken);
			consume(ColonToken);
			node->setVar(fieldName, parseConst());
			skipNewlines();
		}
		doRead();

	} catch (ParseException &e) {
		std::list<Util::TraceBack> traceback;
		traceback.emplace_back(mFile, mCurrentLine, name);
		Util::UtilMethods::log(e.what(), Ogre::LML_CRITICAL, traceback);
		throw;
	}
}

void ScriptParser::parseMethod(const EntityNodePtr &entity)
{
    std::string name = consume(NameToken);

	MethodNodePtr node;

	bool created = false;

    if (!entity.isNull()) {
		if (entity->hasMethod(name)) {
			node = entity->getMethod(name);
		}
    } else {
        auto it = mGlobalMethods.find(name);
        if (it != mGlobalMethods.end()) {
			if (!mReload)
				throw ParseException(Database::Exceptions::doubleGlobalMethodDefinition(name));
			node = it->second;
		}
		
    }
	if (node.isNull()) {

		Ogre::NameValuePairList param;
		param["Type"] = "Method";

		std::string resName = (entity.isNull() ? name : entity->getName() + "::" + name);

		node = createResource(resName, mGroup, false, 0, &param).staticCast<MethodNode>();
		created = true;
		if (!entity.isNull()) {
			entity->addMethod(node, name);
		}
		else {
			mGlobalMethods[name] = node;
		}
	}
	else {
		node->clear();
	}

	node->_notifyOrigin(mFile);
	node->setLineNr(mCurrentLine);

	try {

		consume(BracketOpen);
		if (mNextToken == BracketClose) {
			doRead();
		}
		else {
			bool loop = true;
			while (loop) {
				node->addArgument(consume(NameToken));
				if (mNextToken != CommaToken && mNextToken != BracketClose) throw 0;
				if (mNextToken == BracketClose) loop = false;
				doRead();
			}
		}
		consume(CurlyBracketOpen);
		skipNewlines();
		while (mNextToken != CurlyBracketClose) {
			node->addStatement(parseFullStatement());
			skipNewlines();
		}
		doRead();

	}
	catch (ParseException &e) {
		std::list<Util::TraceBack> traceback;
		traceback.emplace_back(mFile, mCurrentLine, name);
		Util::UtilMethods::log(e.what(), Ogre::LML_CRITICAL, traceback);
		if (created) {
			if (!entity.isNull()) {
				entity->removeMethod(name);
			}
			else {
				mGlobalMethods.erase(name);
			}
			remove(node->getHandle());
		}
		throw;
	}

}


Ogre::unique_ptr<const Statements::Statement> ScriptParser::parseFullStatement()
{
	int line = mCurrentLine;
    bool bReturn = false;
    if (mNextToken == ReturnToken){
        doRead();
        if (mNextToken == NewlineToken) {
            return MAKE_STATEMENT(Statements::Return)(line);
        }
        bReturn = true;
    }

    Ogre::unique_ptr<const Statements::Statement> s = parseTerm();

    bool loop = true;
    while (loop) {
        switch (mNextToken) {
        case BinaryLogical: {
            std::string op = consume();
            skipNewlines();
            Ogre::unique_ptr<const Statements::Statement> temp = parseTerm();
            s = MAKE_STATEMENT(Statements::LogicalOperator)(line, op, std::move(s), std::move(temp));
            break;
        }
        default:
            loop = false;
            break;
        }
    }

    if (bReturn)
        return MAKE_STATEMENT(Statements::Return)(line, std::move(s));
    else
        return s;
}

Ogre::unique_ptr<const Statements::Statement> ScriptParser::parseSingleStatement()
{
	int line = mCurrentLine;
    Ogre::unique_ptr<const Statements::Statement> temp;
    switch (mNextToken) {
    case BracketOpen:
        doRead();
        temp = parseFullStatement();
        consume(BracketClose);
        return temp;
    case LogicalNot:
        doRead();
        temp = parseSingleStatement();
        return MAKE_STATEMENT(Statements::Negate)(line, std::move(temp));
    case IfToken: {
        doRead();
        consume(BracketOpen);
        temp = parseFullStatement();
        consume(BracketClose);
        skipNewlines();
        consume(CurlyBracketOpen);
        skipNewlines();
        std::list<Ogre::unique_ptr<const Statements::Statement>> statements, elseStatements;
        while (mNextToken != CurlyBracketClose) {
            statements.emplace_back(parseFullStatement());
            skipNewlines();
        }
        doRead();
        if (mNextToken == ElseToken) {
            doRead();
			skipNewlines();
			if (mNextToken == IfToken) {
				elseStatements.emplace_back(parseSingleStatement());
			}
			else {
				consume(CurlyBracketOpen);
				skipNewlines();
				while (mNextToken != CurlyBracketClose) {
					elseStatements.emplace_back(parseFullStatement());
					skipNewlines();
				}
				doRead();
				consume(NewlineToken);
			}
		}
		else {
			consume(NewlineToken);
		}

        return MAKE_STATEMENT(Statements::If)(line, std::move(temp), std::move(statements), std::move(elseStatements));
    }
    case WhileToken: {
        doRead();
        consume(BracketOpen);
        temp = parseFullStatement();
        consume(BracketClose);
        skipNewlines();
        consume(CurlyBracketOpen);
        skipNewlines();
        std::list<Ogre::unique_ptr<const Statements::Statement>> statements;
        while (mNextToken != CurlyBracketClose) {
            statements.emplace_back(parseFullStatement());
            skipNewlines();
        }
        doRead();
        consume(NewlineToken);
        return MAKE_STATEMENT(Statements::While)(line, std::move(temp), std::move(statements));
    }
    case ForToken: {
        Ogre::unique_ptr<const Statements::Statement> temp2, temp3;
        doRead();
        consume(BracketOpen);
        temp = parseFullStatement();
        consume(SemikolonToken);
        temp2 = parseFullStatement();
        consume(SemikolonToken);
        temp3 = parseFullStatement();
        consume(BracketClose);
        skipNewlines();
        consume(CurlyBracketOpen);
        skipNewlines();
        std::list<Ogre::unique_ptr<const Statements::Statement>> statements;
        while (mNextToken != CurlyBracketClose) {
            statements.emplace_back(parseFullStatement());
            skipNewlines();
        }
        doRead();
        consume(NewlineToken);
        return MAKE_STATEMENT(Statements::For)(line, std::move(temp), std::move(temp2), std::move(temp3), std::move(statements));
    }
    case NameToken: {
        std::string name = consume();
        switch (mNextToken) {
        case BracketOpen: {
            doRead();
            std::list<Ogre::unique_ptr<const Statements::Statement>> args;
            if (mNextToken == BracketClose) {
                doRead();
            } else {
                bool loop = true;
                while (loop) {
                    args.emplace_back(parseFullStatement());
                    if (mNextToken != CommaToken && mNextToken != BracketClose) throw 0;
                    if (mNextToken == BracketClose) loop = false;
                    doRead();
                }
            }
            switch (mNextToken) {
            case DotToken:
                doRead();
                temp = parseSingleStatement();
                return MAKE_STATEMENT(Statements::ScopeSetter)(line, std::move(temp), MAKE_STATEMENT(Statements::MethodCall)(line, name,
                                                   std::move(args)));
            case ComparatorToken: {
                std::string op = consume();
                temp = parseTerm();
                return MAKE_STATEMENT(Statements::Comparator)(line, op, MAKE_STATEMENT(Statements::MethodCall)(line, name, std::move(args)), std::move(temp));
            }
            default:
                return MAKE_STATEMENT(Statements::MethodCall)(line, name, std::move(args));
            }
        }
        case DotToken:
            doRead();
            temp = parseSingleStatement();
            return MAKE_STATEMENT(Statements::ScopeSetter)(line, std::move(temp), MAKE_STATEMENT(Statements::Accessor)(line, name));
        case AssignmentToken: {
            std::string op = consume();
            temp = parseFullStatement();
            return MAKE_STATEMENT(Statements::Assignment)(line, name, op, std::move(temp));
        }
        case ComparatorToken:  {
            std::string op = consume();
            temp = parseTerm();
            return MAKE_STATEMENT(Statements::Comparator)(line, op, MAKE_STATEMENT(Statements::Accessor)(line, name), std::move(temp));
        }
        default:
            return MAKE_STATEMENT(Statements::Accessor)(line, name);
        }
    }
    case DotToken:
        doRead();
        temp = parseSingleStatement();
        return MAKE_STATEMENT(Statements::ScopeSetter)(line, std::move(temp), MAKE_STATEMENT(Statements::Accessor)(line, ""));
    default:
		return MAKE_STATEMENT(Statements::ConstValue)(line, parseConst());
    }
}

Ogre::unique_ptr<const Statements::Statement> ScriptParser::parseTerm()
{
	int line = mCurrentLine;

	Ogre::unique_ptr<const Statements::Statement> s = parseSingleStatement();

	bool loop = true;
	while (loop) {
		switch (mNextToken) {
		case DivideToken:
		case PlusToken: {
			std::string op = consume();
			skipNewlines();
			Ogre::unique_ptr<const Statements::Statement> temp = parseSingleStatement();
			s = MAKE_STATEMENT(Statements::ArithmeticalOperator)(line, op, std::move(s), std::move(temp));
			break;
		}
		default:
			loop = false;
			break;
		}
	}

	return s;
}

ValueType ScriptParser::parseConst()
{
	switch (mNextToken) {
	case VectorBracketOpen: {
		doRead();
		float x, y, z;

		bool minus = mNextToken == MinusToken;
		if (minus) doRead();
		if (mNextToken != IntToken
			&& mNextToken != FloatToken) throw "Expected numeric value";
		x = (minus ? -1 : 1) * std::stof(consume());

		consume(CommaToken);

		minus = mNextToken == MinusToken;
		if (minus) doRead();
		if (mNextToken != IntToken
			&& mNextToken != FloatToken) throw "Expected numeric value";
		y = (minus ? -1 : 1) * std::stof(consume());

		consume(CommaToken);

		minus = mNextToken == MinusToken;
		if (minus) doRead();
		if (mNextToken != IntToken
			&& mNextToken != FloatToken) throw "Expected numeric value";
		z = (minus ? -1 : 1) * std::stof(consume());

		consume(VectorBracketClose);

		return Ogre::Vector3(x, y, z);
	}
	case StringToken:
		return consume();
	case IntToken:
		return std::stoi(consume());
	case FloatToken:
		return std::stof(consume());
	case BoolToken:
		return consume() == "true";
	case NullToken:
		doRead();
		return ValueType();
	default:
		throw ParseException(Database::Exceptions::invalidExpression(mCurrentLine));
	}
}

void ScriptParser::skipNewlines()
{
    while (mNextToken == NewlineToken) doRead();
}

std::string ScriptParser::consume()
{
    std::string temp = mNextText;
    doRead();
    return temp;
}

std::string ScriptParser::consume(ScriptParser::TokenType expected)
{
    if (mNextToken != expected)
        throw ParseException(Database::Exceptions::unexpectedParseType(typeToString(expected), mStream->getName(), mCurrentLine, mStream->tell() - mLineBase - mNextText.length()));
    return consume();
}

void ScriptParser::doRead()
{
    if (mNextToken == EOFToken) throw 0;

    char c = ' ';

    mNextText.clear();

    while (c == ' ' || c == '\r' || c == '\t') {
        if (!mStream->read(&c, 1)) {
            mNextToken = EOFToken;
            return;
        }
    }

    if (c == '#') {
        while (c != '\n') {
            if (!mStream->read(&c, 1)) {
                mNextToken = EOFToken;
                return;
            }
        }
    }

    mNextText += c;
    if (c == '{') {
        mNextToken = CurlyBracketOpen;
    } else if (c == '}') {
        mNextToken = CurlyBracketClose;
    } else if (c == '[') {
        mNextToken = VectorBracketOpen;
    } else if (c == ']') {
        mNextToken = VectorBracketClose;
    } else if (c == '(') {
        mNextToken = BracketOpen;
    } else if (c == ')') {
        mNextToken = BracketClose;
    } else if (c == ',') {
        mNextToken = CommaToken;
    } else if (c == '.') {
        mNextToken = DotToken;
    } else if (c == '=') {
        mNextToken = AssignmentToken;
        mStream->read(&c, 1);
        if (!mStream->eof()){
            if (c == '='){
                mNextText += c;
                mNextToken = ComparatorToken;
            }else{
                mStream->seek((int)mStream->tell() - 1);
            }
        }
    } else if (c == '"') {
        c = safeRead();
        mNextText.clear();
        while (c != '"') {
            mNextText += c;
            c = safeRead();
        }
        mNextToken = StringToken;
    } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
        mNextText.clear();
        while (!mStream->eof() && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')) {
            mNextText += c;
            mStream->read(&c, 1);
        }
        if (!mStream->eof())
            mStream->seek((int)mStream->tell() - 1);
        if (mNextText == "if") {
            mNextToken = IfToken;
        } else if (mNextText == "while") {
            mNextToken = WhileToken;
        } else if (mNextText == "for") {
            mNextToken = ForToken;
        } else if (mNextText == "else") {
            mNextToken = ElseToken;
        } else if (mNextText == "true" || mNextText == "false") {
            mNextToken = BoolToken;
        } else if (mNextText == "return") {
            mNextToken = ReturnToken;
        } else if (mNextText == "null") {
            mNextToken = NullToken;
        } else {
            mNextToken = NameToken;
        }
    } else if (c == '\n') {
        mNextToken = NewlineToken;
        ++mCurrentLine;
        mLineBase = mStream->tell();
    } else if (c == '&' || c == '|') {
        char oldC = c;
        mNextToken = BinaryLogical;
        c = safeRead();
        if (c != oldC) throw "Unknown Operator";
        mNextText += c;
    } else if (c == '!') {
        mNextToken = LogicalNot;
        mStream->read(&c, 1);
        if (!mStream->eof()){
            if (c == '='){
                mNextText += c;
                mNextToken = ComparatorToken;
            }else{
                mStream->seek((int)mStream->tell() - 1);
            }
        }
    } else if (c >= '0' && c <= '9') {
        mNextText.clear();
        while (!mStream->eof() && (c >= '0' && c <= '9')) {
            mNextText += c;
            mStream->read(&c, 1);
        }
        mNextToken = IntToken;
        if (!mStream->eof()) {
            if (c == '.') {
                do {
                    mNextText += c;
                    mStream->read(&c, 1);
                } while (!mStream->eof() && (c >= '0' && c <= '9'));
                if (!mStream->eof())
                    mStream->seek((int)mStream->tell() - 1);
                mNextToken = FloatToken;
            } else {
                mStream->seek((int)mStream->tell() - 1);
            }
        }
    } else if (c == '-') {
        mNextToken = MinusToken;
		mStream->read(&c, 1);
		if (!mStream->eof()) {
			if (c == '=') {
				mNextText += c;
				mNextToken = AssignmentToken;
			}
			else {
				mStream->seek((int)mStream->tell() - 1);
			}
		}
	}
	else if (c == '+') {
		mNextToken = PlusToken;
		mStream->read(&c, 1);
		if (!mStream->eof()) {
			if (c == '=') {
				mNextText += c;
				mNextToken = AssignmentToken;
			}
			else {
				mStream->seek((int)mStream->tell() - 1);
			}
		}
	}else if (c == '/') {
		mNextToken = DivideToken;
		mStream->read(&c, 1);
		if (!mStream->eof()) {
			if (c == '=') {
				mNextText += c;
				mNextToken = AssignmentToken;
			}
			else {
				mStream->seek((int)mStream->tell() - 1);
			}
		}
    } else if (c == '<' || c == '>'){
        mNextToken = ComparatorToken;
	}
	else if (c == ';') {
		mNextToken = SemikolonToken;
	}
	else if (c == ':') {
		mNextToken = ColonToken;
    } else {
        throw ParseException(std::string("Unknown Character: ") + c);
    }

    //Ogre::LogManager::getSingleton().logMessage(std::string("Read ") + mNextText + " as " + typeToString(mNextToken));

}

char ScriptParser::safeRead()
{
    char c;
    if (!mStream->read(&c, 1)) throw "Unexpected EOF!";
    return c;
}

}
}
}
