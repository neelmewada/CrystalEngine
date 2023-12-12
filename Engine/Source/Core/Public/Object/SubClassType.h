#pragma once

namespace CE
{
    template<typename TBaseClass = CE::Object> requires TIsBaseClassOf<CE::Object, TBaseClass>::Value
	struct SubClassType
	{
	public:

		SubClassType() : baseClassType(TBaseClass::Type()), type(nullptr)
		{

		}

		SubClassType(ClassType* subClassType) : baseClassType(TBaseClass::Type()), type(nullptr)
		{
			if (subClassType == nullptr)
				return;

			if (subClassType->IsSubclassOf(baseClassType))
				this->type = subClassType;

			UpdateClassTypeName();
		}

		SubClassType(const Name& typeName) : baseClassType(TBaseClass::Type()), type(nullptr), classTypeName(typeName)
		{
			if (!typeName.IsValid())
				return;

			ClassType* classType = ClassType::FindClass(typeName);
			if (classType == nullptr)
				return;

			if (classType->IsSubclassOf(baseClassType))
				this->type = classType;

			UpdateClassTypeName();
		}

		inline bool IsValid() const
		{
			UpdateTypePtr();
			return type != nullptr;
		}

		inline operator ClassType*() const
		{
			UpdateTypePtr();
			return type;
		}

		template<typename T>
		inline operator SubClassType<T>&() const
		{
			return *(SubClassType<T>*)((void*)this);
		}

		inline ClassType* GetBaseClassType() const
		{
			return baseClassType;
		}

		inline bool operator==(ClassType* other) const
		{
			return type == other;
		}

		template<typename T>
		inline bool operator==(SubClassType<T> other) const
		{
			return type == other.type;
		}

		template<typename T> requires TIsBaseClassOf<TBaseClass, T>::Value
		inline SubClassType& operator=(const SubClassType<T>& copy)
		{
			if (baseClassType == nullptr)
				baseClassType = TBaseClass::Type();
			if (copy.type == nullptr)
			{
				this->type = nullptr;
			}
			else if (copy.type->IsSubclassOf(baseClassType))
			{
				this->type = copy.type;
			}
			UpdateClassTypeName();
			return *this;
		}

		inline SubClassType& operator=(ClassType* assignType)
		{
			if (baseClassType == nullptr)
				baseClassType = TBaseClass::Type();
			if (assignType == nullptr)
			{
				this->type = nullptr;
			}
			else if (assignType->IsSubclassOf(baseClassType))
			{
				this->type = assignType;
			}

			UpdateClassTypeName();
			return *this;
		}

		inline SubClassType& operator=(const String& typeName)
		{
			if (baseClassType == nullptr)
				baseClassType = TBaseClass::Type();

			if (typeName.IsEmpty())
			{
				this->type = nullptr;
				UpdateClassTypeName();
				return *this;
			}

			ClassType* classType = ClassType::FindClass(typeName);
			if (classType == nullptr)
				return *this;
			if (classType->IsSubclassOf(baseClassType))
				this->type = classType;

			UpdateClassTypeName();
			return *this;
		}

		inline ClassType* operator->() const
		{
			return type;
		}

		template<typename TClass>
		void Set()
		{
			if (baseClassType == nullptr)
				baseClassType = TBaseClass::Type();
			ClassType* assignClass = TClass::Type();
			if (assignClass == nullptr)
			{
				this->type = nullptr;
			}
			else if (assignClass->IsSubclassOf(baseClassType))
			{
				this->type = assignClass;
			}

			UpdateClassTypeName();
		}

	private:

		inline void UpdateTypePtr()
		{
			if (type == nullptr && classTypeName.IsValid())
				type = ClassType::FindClass(classTypeName);
		}

		inline void UpdateClassTypeName()
		{
			if (type != nullptr)
			{
				classTypeName = type->GetTypeName();
			}
			else
			{
				classTypeName = "";
			}
		}

		ClassType* type = nullptr;
		ClassType* baseClassType = nullptr;
		Name classTypeName = "";
	};

} // namespace CE

CE_RTTI_POD_TEMPLATE(CORE_API, CE, SubClassType, CE::Object)
