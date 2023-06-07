#pragma once

namespace CE::Widgets
{

	ENUM()
	enum class CStackDirection
	{
		Horizontal,
		Vertical
	};

	CLASS()
	class COREWIDGETS_API CStackLayout : public CWidget
	{
		CE_CLASS(CStackLayout, CWidget)
	public:

		CStackLayout();
		virtual ~CStackLayout();

		CStackDirection GetDirection() const;
		void SetDirection(CStackDirection direction);

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		FIELD()
		CStackDirection direction{};
	};
    
} // namespace CE::Widgets

#include "CStackLayout.rtti.h"
