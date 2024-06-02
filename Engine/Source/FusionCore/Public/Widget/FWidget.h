#pragma once

namespace CE
{
    CLASS(Abstract)
    class FUSIONCORE_API FWidget : public Object
    {
        CE_CLASS(FWidget, Object)
    public:

        FWidget();

        virtual ~FWidget();

        // - Public API -

        virtual u32 GetSlotCount() { return 0; }

        virtual FSlot* GetSlot(u32 index) { return nullptr; }

        virtual FWidget& operator+(const FSlot& slot);
        virtual FWidget& operator+(FSlot& slot);

    	void AddSlot(const FSlot& slot);

        virtual bool RemoveSlot(FSlot* slot);

        void DestroySlot(FSlot* slot);

    protected:

        void OnAfterConstruct() override;

        void OnBeforeDestroy() override;

        virtual void Construct();

    private:  // - Fields -

        FIELD()
        FSlot* m_Parent = nullptr;

    public:  // - Properties -

        FSlot* GetParent() const { return m_Parent; }

        template<typename TWidget> requires TIsBaseClassOf<FWidget, TWidget>::Value
        TWidget& As()
        {
            TWidget* cast = Object::CastTo<TWidget>(this);
            if (cast == nullptr)
            {
                throw FusionException(String::Format("Cannot cast object ({}) to type {}",
                    GetName(), GetStaticClass<TWidget>()->GetName()), 
                    this);
            }
            return (TWidget&)*cast;
        }

        Self& Name(const Name& newName)
        {
            this->SetName(newName);
            return *this;
        }

        template<typename TWidget> requires TIsBaseClassOf<FWidget, TWidget>::Value
        TWidget& Assign(TWidget*& out)
        {
            out = (TWidget*)this;
            return *out;
        }

    private:


        FUSION_TESTS;
        friend class FSlot;
    };
    
} // namespace CE

#include "FWidget.rtti.h"
