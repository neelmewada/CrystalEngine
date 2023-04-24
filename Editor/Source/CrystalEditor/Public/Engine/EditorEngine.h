#pragma once

#include "GameFramework/Engine.h"

namespace CE::Editor
{

	CLASS()
	class CRYSTALEDITOR_API EditorEngine : public Engine
	{
		CE_CLASS(EditorEngine, CE::Engine)
	public:
		EditorEngine();
		virtual ~EditorEngine();

	};

} // namespace CE::Editor

#include "EditorEngine.rtti.h"
