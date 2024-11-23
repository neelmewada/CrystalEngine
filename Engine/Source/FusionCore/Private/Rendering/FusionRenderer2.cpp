#include "FusionCore.h"

namespace CE
{

    FusionRenderer2::FusionRenderer2()
    {

    }

    FusionRenderer2::~FusionRenderer2()
    {
        
    }

    void FusionRenderer2::Init(const FusionRendererInitInfo& initInfo)
    {

    }

    void FusionRenderer2::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();


    }

    void FusionRenderer2::SetViewConstants(const RPI::PerViewConstants& viewConstants)
    {
        this->viewConstants = viewConstants;

        for (int i = 0; i < viewConstantsUpdateRequired.GetSize(); ++i)
        {
            viewConstantsUpdateRequired[i] = true;
        }
    }

    void FusionRenderer2::Begin()
    {
        ZoneScoped;

        Ref<FFontManager> fontManager = FusionApplication::Get()->GetFontManager();

        currentBrush = FBrush();
        currentPen = FPen();
        currentFont = FFont(fontManager->GetDefaultFontFamily(), fontManager->GetDefaultFontSize());
        itemTransform = Matrix4x4::Identity();

        
        coordinateSpaceStack.RemoveAll();

        PushChildCoordinateSpace(Matrix4x4::Identity());
    }

    void FusionRenderer2::End()
    {
        ZoneScoped;

        PopChildCoordinateSpace();
    }

    void FusionRenderer2::PushChildCoordinateSpace(const Matrix4x4& transform)
    {
        if (coordinateSpaceStack.IsEmpty())
        {
            coordinateSpaceStack.Insert(transform);
        }
        else
        {
            coordinateSpaceStack.Insert(coordinateSpaceStack.Last() * transform);
        }
    }

    void FusionRenderer2::PopChildCoordinateSpace()
    {
        if (!coordinateSpaceStack.IsEmpty())
        {
            coordinateSpaceStack.RemoveLast();
        }
    }

} // namespace CE

