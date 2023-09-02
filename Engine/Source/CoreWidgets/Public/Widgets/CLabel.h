#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CLabel : public CWidget
    {
        CE_CLASS(CLabel, CWidget)
    public:
        
        CLabel();
        virtual ~CLabel();
        
        void SetText(const String& text);
        
        const String& GetText() const;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline bool IsDisabled() const { return isDisabled; }
		inline void SetEnabled() { isDisabled = false; }
		inline void SetDisabled() { isDisabled = true; }

		CE_SIGNAL(OnTextClicked);

    protected:

		void OnAfterComputeStyle() override;
        
        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;
        
    private:
        
        FIELD()
        String text{};

		FIELD()
		b8 isDisabled = false;

		FIELD()
		CWordWrap wordWrap = CWordWrap::Inherited;
    };
    
} // namespace CE::Widgets

#include "CLabel.rtti.h"
