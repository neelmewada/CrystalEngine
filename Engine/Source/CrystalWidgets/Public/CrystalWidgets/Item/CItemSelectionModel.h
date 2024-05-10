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

        bool IsSelected(const CModelIndex& index) const
        {
	        if (!index.IsValid())
                return false;

	        for (int i = 0; i < selectionRanges.GetSize(); ++i)
	        {
                const CModelIndex& start = selectionRanges[i].start;
                const CModelIndex& end = selectionRanges[i].end;

                if (index.GetRow() >= start.GetRow() && index.GetRow() <= end.GetRow() &&
                    index.GetColumn() >= start.GetColumn() && index.GetColumn() <= end.GetColumn())
                {
	                return true;
                }
	        }

            return false;
        }
    };

    CLASS()
    class CRYSTALWIDGETS_API CItemSelectionModel : public Object
    {
        CE_CLASS(CItemSelectionModel, Object)
    public:

        CItemSelectionModel();
        virtual ~CItemSelectionModel();

        b8 HasSelection() const { return hasSelection; }

        bool IsSelected(const CModelIndex& index) const
        {
            return selection.IsSelected(index);
        }

        CBaseItemModel* GetModel() const { return model; }

        void SetModel(CBaseItemModel* model) { this->model = model; }

        void Select(const CModelIndex& start, const CModelIndex& end);

        void Select(const CModelIndex& index);

        void Clear();

        CE_SIGNAL(OnSelectionChanged);

    protected:

        FIELD()
        CBaseItemModel* model = nullptr;

        b8 hasSelection = false;

        CItemSelection selection{};
    };
    
} // namespace CE::Widgets

#include "CItemSelectionModel.rtti.h"