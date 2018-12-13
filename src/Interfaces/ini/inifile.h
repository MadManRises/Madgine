#pragma once

namespace Engine {
	namespace Ini {

		struct INTERFACES_EXPORT IniSection {
			IniSection(std::istream *stream = nullptr);

			std::string &operator[](const std::string &key);

			void save(std::ostream &stream) const;
			void load(std::istream &stream);

			std::map<std::string, std::string>::iterator begin();
			std::map<std::string, std::string>::iterator end();

		private:		

			std::map<std::string, std::string> mValues;

		};

		struct INTERFACES_EXPORT IniFile {

			IniFile(const std::experimental::filesystem::path &path);

			IniSection &operator[](const std::string &key);

			void clear();

			void saveToDisk() const;
			void loadFromDisk();

			const std::experimental::filesystem::path &path();

			std::map<std::string, IniSection>::iterator begin();
			std::map<std::string, IniSection>::iterator end();

		private:
			std::experimental::filesystem::path mPath;

			std::map<std::string, IniSection> mSections;

		};

	}
}