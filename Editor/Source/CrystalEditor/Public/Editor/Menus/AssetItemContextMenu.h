#pragma once

namespace CE::Editor
{
    CLASS()
	class CRYSTALEDITOR_API AssetItemContextMenu : public CContextMenu
	{
		CE_CLASS(AssetItemContextMenu, CContextMenu)
	public:

		AssetItemContextMenu();
		virtual ~AssetItemContextMenu();

		void SetAssets(const Array<Name>& assets);

	protected:

		void Construct() override;

		FUNCTION()
		void OnCommonMenuItemClicked(CMenuItem* menuItem);

		Array<Name> targetAssets{};
		Array<AssetDefinition*> targetAssetDefs{};
	};

} // namespace CE::Editor

#include "AssetItemContextMenu.rtti.h"
