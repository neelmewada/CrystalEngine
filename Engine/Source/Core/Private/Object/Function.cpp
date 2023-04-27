
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

    bool FunctionType::IsEventFunction() const
    {
        return GetLocalAttributes().Exists([](const Attribute& attr) -> bool
            {
                return attr.GetKey() == "Event";
            });
    }

    bool FunctionType::IsSignalFunction() const
    {
        return GetLocalAttributes().Exists([](const Attribute& attr) -> bool
            {
                return attr.GetKey() == "Signal";
            });
    }

} // namespace CE

