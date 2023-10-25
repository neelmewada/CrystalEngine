#include "EditorCore.h"

namespace CE::Editor
{
    AssetDefinitionRegistry::AssetDefinitionRegistry()
    {
        if (!IsDefaultInstance())
        {
            classRegHandle = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&Self::OnClassRegistered, this));
			classDeregHandle = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&Self::OnClassDeregistered, this));
        }
    }

    AssetDefinitionRegistry::~AssetDefinitionRegistry()
    {
        if (classRegHandle != 0)
        {
            CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(classRegHandle);
            classRegHandle = 0;
        }

		if (classDeregHandle != 0)
		{
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(classDeregHandle);
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
			assetDefinitions.Add((AssetDefinition*)classType->GetDefaultInstance());
        }
    }

    void AssetDefinitionRegistry::OnClassDeregistered(ClassType* classType)
    {
        if (!classType)
            return;
        
        if (classType != AssetDefinition::StaticType() &&
            classType->IsSubclassOf<AssetDefinition>() &&
			classType->CanBeInstantiated())
        {
			assetDefinitions.Remove((AssetDefinition*)classType->GetDefaultInstance());
        }
    }

} // namespace CE::Editor

