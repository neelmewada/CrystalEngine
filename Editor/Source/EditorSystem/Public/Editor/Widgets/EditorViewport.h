#pragma once

namespace CE::Editor
{
	CLASS()
	class EDITORSYSTEM_API EditorViewport : public CViewport
	{
		CE_CLASS(EditorViewport, CViewport)
	public:

		EditorViewport();
		~EditorViewport();

	private:

	};

} // namespace CE::Editor

#include "EditorViewport.rtti.h"