#pragma once

namespace CE
{
    class FAbstractItemModel;

    STRUCT()
    struct FUSION_API FModelIndex final
    {
        CE_STRUCT(FModelIndex)
    public:

        FModelIndex() {}

        virtual ~FModelIndex() {}

        bool IsValid() const { return model != nullptr; }

        void* GetDataPtr() const { return data; }

        u32 GetRow() const { return row; }

        u32 GetColumn() const { return col; }

        SIZE_T GetHash() const;

        bool operator==(const FModelIndex& other) const;
        bool operator!=(const FModelIndex& other) const;

    private:

        void* data = nullptr;
        u32 row = 0;
        u32 col = 0;

        FAbstractItemModel* model = nullptr;

        friend class FAbstractItemModel;
    };

    CLASS(Abstract)
    class FUSION_API FAbstractItemModel : public Object
    {
        CE_CLASS(FAbstractItemModel, Object)
    protected:

        FAbstractItemModel();

    public:

        virtual ~FAbstractItemModel();

        // - Public API -

        //! @brief Override this function and return false if the data is not ready or if it's empty.
        //! That will prevent rendering of any content in the item view.
        virtual bool IsReady() { return true; }

        FModelIndex CreateIndex(u32 row, u32 column, void* data = nullptr);

        virtual FModelIndex GetParent(const FModelIndex& index) { return FModelIndex(); }

        virtual FModelIndex GetIndex(u32 row, u32 column, const FModelIndex& parent = {}) = 0;

        virtual u32 GetRowCount(const FModelIndex& parent = {}) = 0;
        virtual u32 GetColumnCount(const FModelIndex& parent = {}) = 0;

        ScriptEvent<void(FAbstractItemModel*)> onModelUpdated;

    };
    
} // namespace CE

#include "FAbstractItemModel.rtti.h"
