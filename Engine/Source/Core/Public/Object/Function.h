#pragma once

#include "Containers/Array.h"
#include "RTTI/RTTI.h"

#include <functional>

namespace CE
{
	class StructType;
	class ClassType;
	class Object;

	using FunctionDelegate = Delegate<void(void* instance, CE::Array<CE::Variant> params, CE::Variant& returnValue)>;

    class CORE_API FunctionType : public TypeInfo
    {
	protected:
		FunctionType(String name, TypeId returnType, std::initializer_list<TypeId> parameterTypes,
			FunctionDelegate delegate, TypeInfo* owner, String attributes,
			TypeId returnUnderlyingTypeId = 0, Array<TypeId> parameterUnderlyingTypeIds = {});

	public:

		virtual const Name& GetTypeName() override;

		virtual bool IsFunction() const override { return true; }

		virtual void InitializeDefaults(void* instance) override {  } // Do nothing
		virtual void CallDestructor(void* instance) override { }

		INLINE TypeId GetReturnTypeId() const { return returnType; }

		INLINE TypeId GetReturnUnderlyingTypeId() const { return returnUnderlyingTypeId; }

		INLINE TypeId GetFunctionSignature()
		{
			if (signature == 0)
				signature = GetCombinedHashes(paramTypes);
			return signature;
		}
		
		INLINE u32 GetParameterCount() const { return paramTypes.GetSize(); }

		INLINE TypeId GetParameterTypeIdAt(u32 index) const { return paramTypes[index]; }

		INLINE TypeId GetParameterUnderlyingTypeIdAt(u32 index) const { return paramUnderlyingTypeIds[index]; }

		INLINE const TypeInfo* GetOwner() const { return owner; }

		INLINE const TypeInfo* GetInstanceOwner() const { return instanceOwner; }

		bool IsEventFunction();
        bool IsSignalFunction();

		virtual TypeId GetTypeId() const override
		{
			return GetReturnTypeId();
		}

		INLINE FunctionType* GetNext() const
		{
			return next;
		}

		INLINE CE::Variant Invoke(void* instance, const Array<CE::Variant>& params)
		{
			CE::Variant returnValue{};
			delegateCallback(instance, params, returnValue);
			return returnValue;
		}

		virtual u32 GetSize() const override { return 0; }

	protected:
		Name typeName{};
		TypeId returnType = 0;
		TypeId returnUnderlyingTypeId = 0;

		Array<TypeId> paramTypes{};
		Array<TypeId> paramUnderlyingTypeIds{};

		FunctionDelegate delegateCallback;
		FunctionType* next = nullptr;
		TypeInfo* owner = nullptr;
		TypeInfo* instanceOwner = nullptr;

		TypeId signature = 0;

		Array<Attribute> attributeList{};

		friend class StructType;
		friend class ClassType;
    };

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	TypeId GetFunctionSignature(ReturnType(ClassOrStruct::* function)(Args...))
	{
		return GetCombinedHashes({ CE::GetTypeId<Args>()...});
	}

	template<typename ReturnType, typename ClassOrStruct, typename... Args>
	TypeId GetFunctionSignature(ReturnType(ClassOrStruct::* function)(Args...) const)
	{
		return GetCombinedHashes({ CE::GetTypeId<Args>()... });
	}

	template<typename... Args>
	TypeId GetFunctionSignature()
	{
		return GetCombinedHashes({ CE::GetTypeId<Args>()... });
	}
    
} // namespace CE

