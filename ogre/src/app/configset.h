#pragma once

#include "serialize/container/map.h"

//#include "Database\translationunit.h"

namespace Engine
{
	namespace App
	{
		/**
		 *
		 */
		class OGREMADGINE_EXPORT ConfigSet : public Ogre::Singleton<ConfigSet>, public Ogre::GeneralAllocatedObject
		{
		public:
			/**
			 *
			 */
			ConfigSet(Ogre::Root* root, const std::string& configFileName);
			ConfigSet(const ConfigSet&) = delete;

			/**
			 * [getFullscreen description]
			 * @return [description]
			 */
			bool getFullscreen();
			/**
			 * [getVSync description]
			 * @return [description]
			 */
			bool getVSync();
			/**
			 * [getResolutionWidth description]
			 * @return [description]
			 */
			int getResolutionWidth();
			/**
			 * [getResolutionHeight description]
			 * @return [description]
			 */
			int getResolutionHeight();
			/**
			 * [getColorDepth description]
			 * @return [description]
			 */
			int getColorDepth();
			/**
			 * [getLanguage description]
			 * @return [description]
			 */
			std::string getLanguage();
			/**
			 * [getRenderSystem description]
			 * @return [description]
			 */
			std::string getRenderSystem();

			/**
			 * [getResolutionString description]
			 * @return [description]
			 */
			std::string getResolutionString();
			/**
			 * [getResolutionAndColorDepthString description]
			 * @return [description]
			 */
			std::string getResolutionAndColorDepthString();

			/**
			 * [setFullscreen description]
			 * @param b [description]
			 */
			void setFullscreen(bool b);
			/**
			 * [setResolutionWidth description]
			 * @param i [description]
			 */
			void setResolutionWidth(int i);
			/**
			 * [setResolutionHeight description]
			 * @param i [description]
			 */
			void setResolutionHeight(int i);
			/**
			 * [setResolutionString description]
			 * @param s [description]
			 */
			void setResolutionString(const std::string& s);
			/**
			 * [setColorDepth description]
			 * @param i [description]
			 */
			void setColorDepth(int i);
			/**
			 * [setLanguage description]
			 * @param language [description]
			 */
			void setLanguage(const std::string& language);
			/**
			 * [applyLanguage description]
			 */
			void applyLanguage();

			/**
			 * [updateWindow description]
			 */
			void updateWindow();

			/**
			 * [save description]
			 */
			void save() const;
			/**
			 * [load description]
			 */
			void load();

			/**
			 * [getPossibleVideoModes description]
			 * @return [description]
			 */
			Ogre::StringVector& getPossibleVideoModes() const;
			/**
			 * [getPossibleColorDepths description]
			 * @return [description]
			 */
			Ogre::StringVector& getPossibleColorDepths() const;


		private:

			static const bool defaultFullScreen;
			static const bool defaultVSync;
			static const int defaultResolutionWidth;
			static const int defaultResolutionHeight;
			static const int defaultColorDepth;
			static const std::string defaultRenderSystem;
			static const std::string defaultLanguage;

			static const std::string fullScreenProperty;
			static const std::string vSyncProperty;
			static const std::string heightProperty;
			static const std::string widthProperty;
			static const std::string colorDepthProperty;
			static const std::string renderSystemProperty;
			static const std::string videoModeProperty;
			static const std::string languageProperty;

			static const std::string yesString;
			static const std::string noString;

		private:
			Serialize::SerializableMap<std::string, ValueType> mSettings;
			Ogre::RenderSystem* mRenderSystem;


			// Database::TranslationUnit mTranslation;

			std::string mConfigFileName;
		};
	}
}
