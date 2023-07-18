#pragma once

namespace CE::Editor
{

	CLASS()
	class EDITORSYSTEM_API EditorEngine : public Engine
	{
		CE_CLASS(EditorEngine, Engine)
	protected:

		EditorEngine();
		virtual ~EditorEngine();

	public:



	protected:


	};
    
} // namespace CE::Editor

#include "EditorEngine.rtti.h"
