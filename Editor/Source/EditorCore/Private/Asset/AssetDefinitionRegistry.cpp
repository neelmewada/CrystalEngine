#include "EditorCore.h"

namespace CE::Editor
{
    AssetDefinitionRegistry::AssetDefinitionRegistry()
    {
        if (!IsDefaultInstance())
        {
            classRegHandle = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&Self::OnClassRegistered, this));
            classDeregHandle = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&Self::OnClassD, <#ClassOrStruct *instance#>))
        }
    }

    AssetDefinitionRegistry::~AssetDefinitionRegistry()
    {
        if (classRegHandle != 0)
        {
            CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(classRegHandle);
            classRegHandle = 0;
            classDeregHandle = 0;
        }
    }

    void AssetDefinitionRegistry::OnClassRegistered(ClassType* classType)
    {
        if (!classType)
            return;
        
        if (classType != AssetDefinition::StaticType() &&
            classType->IsSubclassOf<AssetDefinition>() &&
            classType->CanBeInstantiated())
        {
            
        }
    }

    void AssetDefinitionRegistry::OnClassDeregistered(ClassType* classType)
    {
        if (!classType)
            return;
        
        if (classType != AssetDefinition::StaticType() &&
            classType->IsSubclassOf<AssetDefinition>())
        {
            
        }
    }

} // namespace CE::Editor

