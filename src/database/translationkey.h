#pragma once


namespace Engine
{
	namespace Database
	{
		class MADGINE_CLIENT_EXPORT TranslationKey
		{
		public:
			TranslationKey(const std::string& key);

			operator std::string() const;

		private:
			std::string mKey;
		};
	}
} // namespace Database
