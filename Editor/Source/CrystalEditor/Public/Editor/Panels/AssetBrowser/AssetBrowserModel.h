#pragma once

namespace CE::Editor
{
	using namespace CE::Widgets;

	CLASS()
	class CRYSTALEDITOR_API AssetBrowserTreeModel : public CDataModel
	{
		CE_CLASS(AssetBrowserTreeModel, CDataModel)
	public:

		virtual bool HasHeader() override;

		virtual ClassType* GetWidgetClass(const CModelIndex& index) override;

		virtual u32 GetRowCount(const CModelIndex& parent) override;

		virtual u32 GetColumnCount(const CModelIndex& parent) override;

		virtual CModelIndex GetParent(const CModelIndex& index) override;

		virtual CModelIndex GetIndex(u32 row, u32 col, const CModelIndex& parent) override;

		virtual void SetData(const CModelIndex& index, CWidget* widget) override;

		void SetPathTreeRootNode(PathTreeNode* rootNode);

		virtual void OnIndexSelected(const CModelIndex& index) override;

		CE_SIGNAL(OnSelectionChanged, PathTreeNode*);

	private:

		PathTreeNode* rootNode = nullptr;
	};
    
} // namespace CE::Editor

#include "AssetBrowserModel.rtti.h"
