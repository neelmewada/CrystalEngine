#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CFileSystemModel::CFileSystemModel()
    {
        
    }

    CFileSystemModel::~CFileSystemModel()
    {

    }

    void CFileSystemModel::SetRootDirectory(const IO::Path& rootDir)
    {
        this->rootDirectory = rootDir;

        pathTree.RemoveAll();

        this->rootDirectory.RecursivelyIterateChildren([&](const IO::Path& path)
            {
                String relative = IO::Path::GetRelative(path, rootDirectory).GetString().Replace({ '\\' }, '/');
                if (!relative.StartsWith("/"))
                    relative = "/" + relative;

                pathTree.AddPath(relative);
            });
    }

    u32 CFileSystemModel::GetRowCount(const CModelIndex& parent)
    {
        if (!parent.IsValid())
        {
            return pathTree.GetRootNode()->children.GetSize();
        }

        PathTreeNode* node = (PathTreeNode*)parent.GetInternalData();
        if (node == nullptr)
            return 0;

        return node->children.GetSize();
    }

    u32 CFileSystemModel::GetColumnCount(const CModelIndex& parent)
    {
        return 2;
    }

    CModelIndex CFileSystemModel::GetParent(const CModelIndex& index)
    {
        if (!index.IsValid())
            return CModelIndex();

        PathTreeNode* node = (PathTreeNode*)index.GetInternalData();
        if (node->parent == nullptr || node->parent == pathTree.GetRootNode())
            return CModelIndex();

        int i = 0;
        if (node->parent->parent != nullptr)
        {
            i = node->parent->parent->children.IndexOf(node->parent); // Should never happen
        }

        return CreateIndex(i, 0, node->parent);
    }

    CModelIndex CFileSystemModel::GetIndex(u32 row, u32 column, const CModelIndex& parent)
    {
        if (!parent.IsValid())
        {
            if (row >= pathTree.GetRootNode()->children.GetSize())
                return CModelIndex();
            return CreateIndex(row, column, pathTree.GetRootNode()->children[row]);
        }

        if (parent.GetInternalData() == nullptr)
            return CModelIndex();

        PathTreeNode* node = (PathTreeNode*)parent.GetInternalData();
        if (row >= node->children.GetSize())
            return CModelIndex();

        return CreateIndex(row, column, node->children[row]);
    }

    Variant CFileSystemModel::GetData(const CModelIndex& index, CItemDataUsage usage)
    {
        PathTreeNode* node = (PathTreeNode*)index.GetInternalData();
        if (node == nullptr)
            return Variant();
        
        if (index.GetColumn() == 0)
        {
            if (usage == CItemDataUsage::Display)
            {
                return node->name;
            }
            else if (usage == CItemDataUsage::Decoration)
            {
                return Variant();
            }
        }
        else if (index.GetColumn() == 1) // Type column
        {
            if (usage == CItemDataUsage::Display)
            {
                return node->IsTerminal() && node->name.GetString().Contains('.') ? "File" : "Folder";
            }
            else if (usage == CItemDataUsage::Decoration)
            {
                return Variant();
            }
        }
        return Variant();
    }

    Variant CFileSystemModel::GetHeaderData(int position, COrientation orientation, CItemDataUsage usage)
    {
        if (usage == CItemDataUsage::Display && orientation == COrientation::Horizontal)
        {
            if (position == 0)
                return "Name";
            else if (position == 1)
                return "Type";
        }
        
        return Variant();
    }

} // namespace CE::Widgets
