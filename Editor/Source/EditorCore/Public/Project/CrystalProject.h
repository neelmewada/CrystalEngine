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

		
	};

} // namespace CE::Editor


#include "CrystalProject.rtti.h"
