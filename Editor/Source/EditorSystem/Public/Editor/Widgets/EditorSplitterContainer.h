#pragma once

namespace CE::Editor
{
	class EditorSplitter;

	CLASS()
	class EDITORSYSTEM_API EditorSplitterContainer : public CWidget
	{
		CE_CLASS(EditorSplitterContainer, CWidget)
	public:

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		bool IsLayoutCalculationRoot() override;

	protected:

		void Construct() override;


		FIELD(ReadOnly)
		EditorSplitter* owner = nullptr;

		FIELD(ReadOnly)
		f32 splitRatio = 0.5f;

		friend class EditorSplitter;
	};

} // namespace CE::Editor

#include "EditorSplitterContainer.rtti.h"