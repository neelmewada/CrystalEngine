#pragma once

namespace CE::Widgets
{
    struct CItemSelectionRange
    {
        CModelIndex start{};
        CModelIndex end{};
    };

    struct CRYSTALWIDGETS_API CItemSelection
    {
        Array<CItemSelectionRange> selectionRanges{};

        void ClearAll()
        {
            selectionRanges.Clear();
        }

        void Select(const CModelIndex& start, const CModelIndex& end)
        {
            selectionRanges.Add({ start, end });
        }

        bool IsSelected(const CModelIndex& index) const;
    };

    CLASS()
    class CRYSTALWIDGETS_API CItemSelectionModel : public Object
    {
        CE_CLASS(CItemSelectionModel, Object)
    public:

        CItemSelectionModel();
        virtual ~CItemSelectionModel();

        b8 HasSelection() const { return hasSelection; }

        const CItemSelection& GetSelection() const { return selection; }

        bool IsSelected(const CModelIndex& index) const
        {
            return selection.IsSelected(index);
        }

        CBaseItemModel* GetModel() const { return model; }

        void SetModel(CBaseItemModel* model) { this->model = model; }

        void Select(const CModelIndex& start, const CModelIndex& end);

        void Select(const CModelIndex& index);

        void Clear();

        // - Events -

        FIELD()
        CVoidEvent onSelectionChanged{};

    protected:

        FIELD()
        CBaseItemModel* model = nullptr;

        b8 hasSelection = false;

        CItemSelection selection{};
    };
    
} // namespace CE::Widgets

#include "CItemSelectionModel.rtti.h"