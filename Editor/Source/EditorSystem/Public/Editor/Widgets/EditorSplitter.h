#pragma once

namespace CE::Editor
{

	CLASS()
	class EDITORSYSTEM_API EditorSplitter : public CWidget
	{
		CE_CLASS(EditorSplitter, CWidget)
	public:

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		bool IsLayoutCalculationRoot() override;

		Vec2 GetAvailableSizeForChild(CWidget* childWidget) override;

		COrientation GetOrientation() const { return orientation; }

		void SetOrientation(COrientation orientation);

	protected:

		void Construct() override;

		FIELD(ReadOnly)
		Array<EditorSplitterContainer*> children{};

		FIELD()
		COrientation orientation = COrientation::Horizontal;

		friend class EditorSplitterContainer;
	};

} // namespace CE::Editor

#include "EditorSplitter.rtti.h"