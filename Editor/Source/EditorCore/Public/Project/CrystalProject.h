#pragma once

namespace CE::Editor
{
    
	STRUCT()
	struct EDITORCORE_API CrystalProject
	{
		CE_STRUCT(CrystalProject)
	public:

		FIELD()
		Name projectName = "Untitled";
        
        FIELD()
        String engineVersion = CE_ENGINE_VERSION_STRING;

	};

} // namespace CE::Editor


#include "CrystalProject.rtti.h"
