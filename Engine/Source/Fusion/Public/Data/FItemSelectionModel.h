#pragma once

namespace CE
{
    ENUM()
    enum class FSelectionMode
    {
	    None = 0,
        Single,
        Multiple,
        Extended
    };
    ENUM_CLASS(FSelectionMode);

    ENUM()
    enum class FSelectionType
    {
	    Row = 0,
        Cell,
        Column
    };
    ENUM_CLASS(FSelectionType)

    CLASS()
    class FUSION_API FItemSelectionModel : public Object
    {
        CE_CLASS(FItemSelectionModel, Object)
    protected:

        FItemSelectionModel();

    public:

        virtual ~FItemSelectionModel();

        // - Public API -

        void Select(const FModelIndex& index);

        bool IsSelected(const FModelIndex& index);

        void SetSelectionMode(FSelectionMode selectionMode);

        void ClearSelection();

    protected:

        void ValidateSelection();

        HashSet<FModelIndex> selection{};
        FSelectionMode selectionMode = FSelectionMode::None;
        FSelectionType selectionType = FSelectionType::Row;

        friend class FTreeView;
        friend class FTreeViewContainer;
    };
    
} // namespace CE

#include "FItemSelectionModel.rtti.h"
