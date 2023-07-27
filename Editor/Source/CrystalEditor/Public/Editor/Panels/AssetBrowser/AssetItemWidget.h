#pragma once

namespace CE::Editor
{
	using namespace CE::Widgets;

    CLASS()
    class CRYSTALEDITOR_API AssetItemWidget : public CButton
    {
        CE_CLASS(AssetItemWidget, CButton)
    public:

        AssetItemWidget();

        virtual ~AssetItemWidget();

		bool IsContainer() override { return true; }

		inline bool IsAsset() const { return isAssetItem; }
		inline bool IsFolder() const { return !isAssetItem; }

		inline const Name& GetPath() const { return path; }
		inline void SetPath(const Name& path) { this->path = path; }

		void SetAsFolder();
		void SetAsAsset();

		inline const String& GetLabel() const { return label; }
		inline void SetLabel(const String& label) { this->label = label; }

		CE_SIGNAL(OnItemDoubleClicked);

    protected:

		void OnBeforeComputeStyle() override;

		void Construct() override;

        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		void OnClicked() override;

	private:

		FIELD()
		String label{};

		GUI::GuiStyleState hovered{};
		GUI::GuiStyleState selected{};

		CTexture icon{};

		Name path{};
		Color bottomFrameColor{};

		b8 isAssetItem = false;
    };
    
} // namespace CE

#include "AssetItemWidget.rtti.h"
