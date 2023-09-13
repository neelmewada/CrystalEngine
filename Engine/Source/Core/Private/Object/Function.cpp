
#include "CoreMinimal.h"

namespace CE
{

    FunctionType::FunctionType(String name, 
        TypeId returnType, 
        std::initializer_list<TypeId> parameterTypes, 
        FunctionDelegate delegate, 
        TypeInfo* owner, 
        String attributes,
		TypeId returnUnderlyingTypeId, 
		Array<TypeId> parameterUnderlyingTypeIds)
        : TypeInfo(name, attributes)
        , returnType(returnType)
        , paramTypes(parameterTypes)
        , delegateCallback(delegate)
		, owner(const_cast<TypeInfo*>(owner))
		, returnUnderlyingTypeId(returnUnderlyingTypeId)
		, paramUnderlyingTypeIds(parameterUnderlyingTypeIds)
    {
		if (paramUnderlyingTypeIds.GetSize() != paramTypes.GetSize())
		{
			paramUnderlyingTypeIds.Resize(paramTypes.GetSize(), 0);
		}
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

