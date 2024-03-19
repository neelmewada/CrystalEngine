#pragma once


namespace CE::Widgets
{
	class CDockWindow;

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

	private:

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

		friend class CDockSpace;
	};

	CLASS()
	class CRYSTALWIDGETS_API CDockSpace : public CWindow
	{
		CE_CLASS(CDockSpace, CWindow)
	public:

		CDockSpace();
		virtual ~CDockSpace();

		CDockSplitView* GetLastDockSplit() const { return dockSplits.Top(); }

		bool Split(CDockSplitView* originalSplitView, f32 splitRatio, CDockSplitDirection splitDirection);

		bool Split(f32 splitRatio, CDockSplitDirection splitDirection);

		CDockType GetDockType() const { return dockType; }

	private:

		void OnSubobjectAttached(Object* object) override;
		void OnSubobjectDetached(Object* object) override;

		void OnPaint(CPaintEvent* paintEvent) override;

		FIELD()
		Array<CDockSplitView*> dockSplits{};

		FIELD()
		CDockType dockType = CDockType::Major;
	};

} // namespace CE::Widgets

#include "CDockSpace.rtti.h"