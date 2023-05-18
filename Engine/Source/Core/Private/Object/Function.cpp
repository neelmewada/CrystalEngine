
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
		, owner(const_cast<TypeInfo*>(owner))
    {

    }

    const Name& FunctionType::GetTypeName()
    {
		if (!typeName.IsValid())
		{
			typeName = owner->GetTypeName().GetString() + "::" + name.GetString();
		}
		return typeName;
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

