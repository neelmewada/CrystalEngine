#pragma once

namespace CE
{
    class FAbstractItemModel;

    struct FModelIndex
    {
    public:

        FModelIndex() {}

        bool IsValid() const { return model != nullptr; }

        void* GetDataPtr() const { return data; }

        u32 GetRow() const { return row; }

        u32 GetColumn() const { return col; }

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

        FModelIndex CreateIndex(u32 row, u32 column, void* data = nullptr);

        virtual FModelIndex GetParent(const FModelIndex& index) { return FModelIndex(); }

        virtual FModelIndex GetIndex(u32 row, u32 column, const FModelIndex& parent = {}) = 0;

        virtual u32 GetRowCount(const FModelIndex& parent = {}) = 0;
        virtual u32 GetColumnCount(const FModelIndex& parent = {}) = 0;



        ScriptEvent<void(FAbstractItemModel*)> onModelUpdated;

    };

    template<>
    inline SIZE_T GetHash<FModelIndex>(const FModelIndex& value)
    {
        return GetCombinedHashes({ (SIZE_T)value.GetRow(), (SIZE_T)value.GetColumn(), (SIZE_T)value.GetDataPtr() });
    }
    
} // namespace CE

#include "FAbstractItemModel.rtti.h"
