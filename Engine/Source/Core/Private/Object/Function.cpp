
#include "Object/Function.h"

namespace CE
{

    FunctionType::FunctionType(String name, 
        TypeId returnType, 
        std::initializer_list<TypeId> parameterTypes, 
        FunctionDelegate delegate, 
        const TypeInfo* owner, 
        String attributes)
        : TypeInfo(name)
        , returnType(returnType)
        , paramTypes(parameterTypes)
        , attributes(attributes)
        , delegateCallback(delegate)
        , owner(owner)
    {

    }

    bool FunctionType::IsEvent() const
    {
        return GetAttributes().Exists([](const Attribute& attr) -> bool
            {
                return attr.GetKey() == "Event";
            });
    }

} // namespace CE

