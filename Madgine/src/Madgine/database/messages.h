#pragma once

#include "../generic/message.h"
#include "translationkey.h"

namespace Engine
{
	namespace Database
	{
		namespace Messages
		{
			const auto unknownKey = message("No Translation available for Key '", "'.");
			const auto loadingMessage = message("Loading ", " ...");

			const TranslationKey quitConfirmMessage("QuitConfirm");
			const std::string noLanguageFile("There is no language file!");
		}
	}
}
