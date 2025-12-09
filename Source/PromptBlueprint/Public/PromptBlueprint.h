// Copyright Sumit Das 2025. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FPromptBlueprintModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
