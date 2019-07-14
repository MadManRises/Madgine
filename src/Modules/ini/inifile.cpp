#include "../moduleslib.h"

#include "inifile.h"

#include "Interfaces/stringutil.h"

#include "Interfaces/filesystem/api.h"

namespace Engine {
	namespace Ini {



		IniSection::IniSection(std::istream *stream)			
		{
			if (stream)
				load(*stream);
		}

		std::string &IniSection::operator[](const std::string &key) {
			return mValues[key];
		}

		void IniSection::save(std::ostream & stream) const
		{
			for (const std::pair<const std::string, std::string> &p : mValues) {
				stream << p.first << "=" << p.second << '\n';
			}
		}

		void IniSection::load(std::istream & stream)
		{
			mValues.clear();
			std::streampos save = stream.tellg();
			std::string line;
			while (std::getline(stream, line)) {
				if (line.at(0) == '[') {
					stream.seekg(save);
					return;
				}
				size_t pos = line.find('=');
				if (pos == std::string::npos || line.rfind('=') != pos)
					throw 0;
				mValues[line.substr(0, pos)] = line.substr(pos + 1);
				save = stream.tellg();
			}
		}

		std::map<std::string, std::string>::iterator IniSection::begin()
		{
			return mValues.begin();
		}

		std::map<std::string, std::string>::iterator IniSection::end()
		{
			return mValues.end();
		}

		IniFile::IniFile(const Filesystem::Path &path) :
			mPath(path)
		{
			loadFromDisk();
		}

		IniSection &IniFile::operator[](const std::string &key)
		{
			return mSections[key];
		}

		void IniFile::clear()
		{
			mSections.clear();
		}

		void IniFile::saveToDisk() const
		{
			std::ofstream stream((mPath.isAbsolute() ? mPath : Filesystem::configPath() / mPath).str());
			assert(stream);
			for (const std::pair<const std::string, IniSection> &p : mSections) {
				stream << "[" << p.first << "]\n";
				p.second.save(stream);
			}
		}

		void IniFile::loadFromDisk()
		{
			mSections.clear();
			std::ifstream stream((mPath.isAbsolute() ? mPath : Filesystem::configPath() / mPath).str());
			std::string line;
			while (std::getline(stream, line)) {
				if (!StringUtil::startsWith(line, "[") || !StringUtil::endsWith(line, "]"))
					throw 0;
				auto pib = mSections.try_emplace(line.substr(1, line.size() - 2), &stream);
			}
		}

		const Filesystem::Path &IniFile::path()
		{
			return mPath;
		}

		std::map<std::string, IniSection>::iterator IniFile::begin()
		{
			return mSections.begin();
		}

		std::map<std::string, IniSection>::iterator IniFile::end()
		{
			return mSections.end();
		}

	}
}