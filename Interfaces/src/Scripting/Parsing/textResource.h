#pragma once



namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT TextResource {
public:
	TextResource(const std::string &name, const std::string &origin);
	TextResource(const TextResource &) = delete;
	virtual ~TextResource();    

	int lineNr();
	void setLineNr(int line);

	const std::string &getOrigin() const;
	const std::string &getName() const;

protected:
	const std::string mName;

private:
	int mLineNr;
	std::string mOrigin;

};

}
}
}


