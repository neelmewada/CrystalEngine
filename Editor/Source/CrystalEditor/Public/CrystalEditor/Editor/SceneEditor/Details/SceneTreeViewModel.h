#pragma once

namespace CE
{
    CLASS()
    class CRYSTALEDITOR_API SceneTreeViewModel : public FTreeViewModel
    {
        CE_CLASS(SceneTreeViewModel, FTreeViewModel)
    public:

        virtual ~SceneTreeViewModel();

    protected:

        SceneTreeViewModel();

	public:

        FModelIndex GetIndex(u32 row, u32 column, const FModelIndex& parent) override;

        u32 GetRowCount(const FModelIndex& parent) override;

        u32 GetColumnCount(const FModelIndex& parent) override;

        void SetData(u32 row, FTreeViewRow& rowWidget, const FModelIndex& parent) override;

        void SetScene(CE::Scene* scene);

    protected:

        CE::Scene* scene = nullptr;

    };
    
} // namespace CE

#include "SceneTreeViewModel.rtti.h"
