#pragma once

namespace CE::Editor
{
    
	STRUCT()
	struct EDITORCORE_API CrystalProject
	{
		CE_STRUCT(CrystalProject)
	public:

		FIELD()
		String projectName{};
        
        FIELD()
        String engineVersion = "0.1.0";
	};

} // namespace CE::Editor


#include "CrystalProject.rtti.h"
