#pragma once

namespace Engine {

struct MODULES_EXPORT CLIOptionBase {

	CLIOptionBase(size_t minArgumentCount, size_t maxArgumentCount, std::vector<const char*> options, const char *help = nullptr);

	virtual bool parse(const std::vector<const char*> &args) = 0;
	virtual const char *typeName() = 0;

	void init();
	std::string help();
	const std::vector<const char*> &options();

private:
	std::vector<const char*> mOptions;
	const char *mHelp = nullptr;
	size_t mMinArgumentCount, mMaxArgumentCount;

	bool mInitialized = false;
};

template <typename T>
struct CLIOptionImpl : CLIOptionBase {

	CLIOptionImpl(std::vector<const char*> options, T defaultValue = {}, const char *help = nullptr) :
		CLIOptionBase(std::is_same_v<T, bool> ? 0 : 1, 1, std::move(options), help),
		mValue(std::move(defaultValue))
	{}

	MODULES_EXPORT bool parse(const std::vector<const char*> &args) override;

	const char *typeName() override {
		return std::is_same_v<T, std::string> ? "string" : typeid(T).name();
	}

	const T &operator*() {
		init();
		return mValue;
	}

	const T* operator->() {
		init();
		return &mValue;
	}

	operator const T&() {
		init();
		return mValue;
	}

private:
	T mValue;
};

template <typename T>
struct CLIOption : CLIOptionImpl<T> {
	using CLIOptionImpl<T>::CLIOptionImpl;
};

template <>
struct CLIOption<std::string> : CLIOptionImpl<std::string> {
	using CLIOptionImpl<std::string>::CLIOptionImpl;

	operator const char *() {
		return static_cast<const std::string&>(*this).c_str();
	}
};

struct MODULES_EXPORT CLI {

	CLI(int argc, char **argv);

	std::string help();

	static const CLI &getSingleton();
	static std::vector<CLIOptionBase*> &options();

	std::map<std::string_view, std::vector<const char*>> mArguments;
};

}