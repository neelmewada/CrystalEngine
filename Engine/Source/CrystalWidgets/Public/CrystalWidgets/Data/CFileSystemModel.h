#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CFileSystemModel : public CBaseItemModel
    {
        CE_CLASS(CFileSystemModel, CBaseItemModel)
    public:

        CFileSystemModel();

        virtual ~CFileSystemModel();

        void SetRootDirectory(const IO::Path& rootDir);

        u32 GetRowCount(const CModelIndex& parent) override;
        u32 GetColumnCount(const CModelIndex& parent) override;
        CModelIndex GetParent(const CModelIndex& index) override;
        CModelIndex GetIndex(u32 row, u32 column, const CModelIndex& parent) override;
        Variant GetData(const CModelIndex& index, CItemDataUsage usage) override;
        Variant GetHeaderData(int position, COrientation orientation, CItemDataUsage usage) override;

    protected:

    private:

        IO::Path rootDirectory{};

        PathTree pathTree{};

    };
    
} // namespace CE::Widgets

#include "CFileSystemModel.rtti.h"