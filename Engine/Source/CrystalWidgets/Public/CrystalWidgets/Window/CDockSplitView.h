#pragma once

namespace CE::Widgets
{

	class CDockWindow;
	class CDockSpace;

	ENUM()
	enum class CDockSplitDirection
	{
		Vertical = 0,
		Horizontal
	};
	ENUM_CLASS(CDockSplitDirection);

	ENUM()
	enum class CDockType
	{
		Major = 0,
		Minor
	};
	ENUM_CLASS(CDockType);

	CLASS()
	class CRYSTALWIDGETS_API CDockSplitView : public CWidget
	{
		CE_CLASS(CDockSplitView, CWidget)
	public:

		CDockSplitView();
		virtual ~CDockSplitView();

		CDockSplitDirection GetSplitDirection() const { return splitDirection; }

		f32 GetSplitRatio() const { return splitRatio; }

		CDockSplitView* FindSplit(Name splitName);

		bool IsLayoutCalculationRoot() override { return true; }

	crystalwidgets_internal:

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		FIELD()
		CDockSplitView* parentSplitView = nullptr;

		FIELD()
		Array<CDockSplitView*> children{};

		FIELD()
		CDockSplitDirection splitDirection = CDockSplitDirection::Vertical;

		FIELD()
		f32 splitRatio = 1.0f;

		FIELD()
		CDockSpace* dockSpace = nullptr;

		friend class CDockSpace;
	};

} // namespace CE::Widgets

#include "CDockSplitView.rtti.h"