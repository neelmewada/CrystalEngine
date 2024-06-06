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

        virtual SubClass<FLayoutSlot> GetSlotClass() const { return nullptr; }

        virtual u32 GetSlotCount() { return 0; }

        virtual FLayoutSlot* GetSlot(u32 index) { return nullptr; }

        virtual FWidget& operator+(const FLayoutSlot& slot);
        virtual FWidget& operator+(FLayoutSlot& slot);

    	void AddLayoutSlot(const FLayoutSlot& slot);

        virtual bool RemoveLayoutSlot(FLayoutSlot* slot);

        void DestroyLayoutSlot(FLayoutSlot* slot);

        FFusionContext* GetContext();

        virtual Vec2 PrecomputeLayoutSize();

    protected:

        virtual void OnFusionPropertyModified(const Name& propertyName) {}

        void OnAfterConstruct() override;

        void OnBeforeDestroy() override;

        virtual void Construct();

        FIELD()
        Vec2 precomputedSize{};

    private:  // - Fields -

        FIELD()
        FLayoutSlot* parent = nullptr;

        FIELD()
        bool layoutDirty = true;

        FIELD()
        FFusionContext* context = nullptr;

    public:  // - Properties -

        FLayoutSlot* GetParent() const { return parent; }

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


        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FWidget.rtti.h"
