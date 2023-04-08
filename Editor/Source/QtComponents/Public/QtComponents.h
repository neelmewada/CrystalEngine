#pragma once

#include "CoreMinimal.h"

#include "QtComponents/StyleManager.h"

// Input Fields
#include "QtComponents/Input/StringField.h"
#include "QtComponents/Input/Vec4Input.h"
#include "QtComponents/Input/EnumField.h"

// Widgets
#include "QtComponents/Widgets/CardWidget.h"
#include "QtComponents/Widgets/AddComponentWidget.h"
#include "QtComponents/Widgets/GridWidget.h"

namespace CE::Editor::Qt
{
    
    class QTCOMPONENTS_API QtComponentsModule : public CE::Module
    {
    public:
        
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
        
    };

} // namespace CE::Editor::Qt
