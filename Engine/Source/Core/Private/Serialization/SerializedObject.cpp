
#include "CoreMinimal.h"

namespace CE
{
    
    SerializedObject::SerializedObject(const TypeInfo* targetType, void* targetInstance)
    {
        if (targetType == nullptr)
            return;
        this->targetType = const_cast<TypeInfo*>(targetType);
        this->targetInstance = targetInstance;
    }

} // namespace CE

