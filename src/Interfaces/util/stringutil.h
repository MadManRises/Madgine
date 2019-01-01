#pragma once

namespace Engine {
	namespace StringUtil {

		inline bool startsWith(const std::string &s, const std::string &prefix) {
			return s.substr(0, prefix.size()) == prefix;
		}

		inline bool endsWith(const std::string &s, const std::string &suffix) {
			return s.size() >= suffix.size() ? (s.substr(s.size() - suffix.size(), suffix.size()) == suffix) : false;
		}

		inline std::string & replace(std::string &s, char old, char replace) {
			for (char &c : s)
			{
				if (c == old) c = replace;
			}
			return s;
		}

		inline std::string replace(std::string &&s, char old, char replace) {
			for (char &c : s)
			{
				if (c == old) c = replace;
			}
			return std::move(s);
		}

		inline std::string toLower(std::string &&s) {
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
			return std::move(s);
		}

		inline std::string toUpper(std::string &&s) {
			std::transform(s.begin(), s.end(), s.begin(), ::toupper);
			return std::move(s);
		}

	}
}