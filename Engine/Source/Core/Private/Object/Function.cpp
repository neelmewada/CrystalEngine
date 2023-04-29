
#include "CoreMinimal.h"

namespace CE
{

    FunctionType::FunctionType(String name, 
        TypeId returnType, 
        std::initializer_list<TypeId> parameterTypes, 
        FunctionDelegate delegate, 
        const TypeInfo* owner, 
        String attributes)
        : TypeInfo(name, attributes)
        , returnType(returnType)
        , paramTypes(parameterTypes)
        , delegateCallback(delegate)
        , owner(owner)
    {

    }

    bool FunctionType::IsEventFunction()
    {
        return HasAttribute("Event");
    }

    bool FunctionType::IsSignalFunction()
    {
        return HasAttribute("Signal");
    }

} // namespace CE

