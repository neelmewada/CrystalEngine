#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneDataModel final : public CBaseItemModel
	{
		CE_CLASS(SceneDataModel, CBaseItemModel)
	public:

		SceneDataModel();
		virtual ~SceneDataModel();

		u32 GetRowCount(const CModelIndex& parent) override;
		u32 GetColumnCount(const CModelIndex& parent) override;
		CModelIndex GetParent(const CModelIndex& index) override;
		CModelIndex GetIndex(u32 row, u32 column, const CModelIndex& parent) override;
		Variant GetData(const CModelIndex& index, CItemDataUsage usage) override;
		Variant GetHeaderData(int position, COrientation orientation, CItemDataUsage usage) override;

		void SetScene(CE::Scene* scene);

		CE::Scene* GetScene() const { return scene; }

	private:

		FIELD()
		CE::Scene* scene = nullptr;

	};

} // namespace CE::Editor

#include "SceneDataModel.rtti.h"