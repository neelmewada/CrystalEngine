#include "CrystalEditor.h"

namespace CE::Editor
{
	AssetItemContextMenu::AssetItemContextMenu()
	{

	}

	AssetItemContextMenu::~AssetItemContextMenu()
	{

	}

	void AssetItemContextMenu::SetAssets(const Array<Name>& assets)
	{
		DestroyAllSubWidgets();

		targetAssets.Clear();
		targetAssetDefs.Clear();

		for (const auto& assetPath : assets)
		{
			auto assetData = AssetRegistry::Get()->GetPrimaryAssetByPath(assetPath);
			if (assetData != nullptr)
			{
				auto targetAssetDef = AssetDefinitionRegistry::Get()->FindAssetDefinitionForAssetClassType(assetData->assetClassPath);
				if (targetAssetDef != nullptr)
				{
					targetAssets.Add(assetPath);
					targetAssetDefs.Add(targetAssetDef);
				}
			}
		}

		Construct();
	}

	void AssetItemContextMenu::Construct()
	{
		Super::Construct();

		auto commonHeaderItem = CreateWidget<CMenuItemHeader>(this, "CommonHeader");
		commonHeaderItem->SetTitle("COMMON");
		{
			auto editItem = CreateWidget<CMenuItem>(this, "EditMenuItem");
			editItem->SetText("Edit");
			editItem->LoadIcon("Icons/edit.png");
			Object::Bind(editItem, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), this, MEMBER_FUNCTION(Self, OnCommonMenuItemClicked));


		}
	}

	void AssetItemContextMenu::OnCommonMenuItemClicked(CMenuItem* menuItem)
	{
		if (menuItem == nullptr)
			return;

		if (menuItem->GetName() == "EditMenuItem")
		{

		}
	}

} // namespace CE::Editor
