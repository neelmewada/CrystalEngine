#pragma once

#include "Containers/Array.h"
#include "RTTI.h"

#include <functional>

namespace CE
{
	class StructType;
	class ClassType;
	class Object;

	template<class _Fty>
	using Function = std::function<_Fty>;

	using FunctionDelegate = Function<void(CE::Object* instance, CE::Array<CE::Variant> params, CE::Variant& returnValue)>;

    class CORE_API FunctionType : public TypeInfo
    {
	protected:
		FunctionType(String name, TypeId returnType, std::initializer_list<TypeId> parameterTypes,
			FunctionDelegate delegate, const TypeInfo* owner, String attributes);

	public:

		virtual bool IsFunction() const override { return true; }

		CE_INLINE TypeId GetReturnTypeId() const { return returnType; }

		CE_INLINE TypeId GetFunctionSignature() const
		{
			return GetCombinedHashes(paramTypes);
		}
		
		CE_INLINE u32 GetParameterCount() const { return paramTypes.GetSize(); }

		CE_INLINE TypeId GetParameterTypeIdAt(u32 index) const { return paramTypes[index]; }

		CE_INLINE const TypeInfo* GetOwner() const { return owner; }

		bool IsEventFunction() const;
        bool IsSignalFunction() const;

		virtual TypeId GetTypeId() const override
		{
			return GetReturnTypeId();
		}

		CE_INLINE FunctionType* GetNext() const
		{
			return Next;
		}

		CE_INLINE CE::Variant Invoke(CE::Object* instance, const Array<CE::Variant>& params) const
		{
			CE::Variant returnValue{};
			delegateCallback(instance, params, returnValue);
			return returnValue;
		}

		virtual u32 GetSize() const override { return 0; }

	protected:
		TypeId returnType = 0;
		Array<TypeId> paramTypes{};
		FunctionDelegate delegateCallback;
		FunctionType* Next = nullptr;
		const TypeInfo* owner = nullptr;

		Array<Attribute> attributeList{};

		friend class StructType;
		friend class ClassType;
    };

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	TypeId GetFunctionSignature(ReturnType(ClassOrStruct::* function)(Args...))
	{
		return GetCombinedHashes({ TYPEID(Args)... });
	}

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	TypeId GetFunctionSignature(ReturnType(ClassOrStruct::* function)(Args...) const)
	{
		return GetCombinedHashes({ TYPEID(Args)... });
	}
    
} // namespace CE

