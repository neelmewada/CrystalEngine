#pragma once

#define FNewOwned(WidgetClass, Parent)\
	(*CreateObject<WidgetClass>(Parent, #WidgetClass))

#define FNew(WidgetClass)\
	(*CreateObject<WidgetClass>(this, #WidgetClass))

#define FAssignNew(WidgetClass, VariableName) FNew(WidgetClass).Assign(VariableName)

#define FAssignNewOwned(WidgetClass, VariableName, Parent) FNewOwned(WidgetClass, Parent).Assign(VariableName)

#define __FUSION_PROPERTY(PropertyType, PropertyName, DirtyFunc)\
	protected:\
		PropertyType m_##PropertyName = {};\
	public:\
		Self& PropertyName(const PropertyType& value) {\
			if constexpr (TEquitable<PropertyType>::Value)\
			{\
				if (TEquitable<PropertyType>::AreEqual(this->m_##PropertyName, value))\
					return *this;\
			}\
			this->m_##PropertyName = value; DirtyFunc;\
			thread_local const CE::Name nameValue = #PropertyName;\
			OnFusionPropertyModified(nameValue);\
			return *this;\
		}\
		const auto& PropertyName() const { return this->m_##PropertyName; }

#define FUSION_LAYOUT_PROPERTY(PropertyType, PropertyName, ...) __FUSION_PROPERTY(PropertyType, PropertyName, MarkLayoutDirty())

#define FUSION_PROPERTY(PropertyType, PropertyName, ...) __FUSION_PROPERTY(PropertyType, PropertyName, MarkDirty())

#define FUSION_DATA_PROPERTY(PropertyType, PropertyName, ...) __FUSION_PROPERTY(PropertyType, PropertyName, MarkDirty())\
	PropertyBinding<PropertyType> m_##PropertyName##Binding{};\
	void Update_##PropertyName() {\
		if (m_##PropertyName##Binding.read.IsBound()) PropertyName(m_##PropertyName##Binding.read());\
	}\
	Self& Bind_##PropertyName(const ScriptDelegate<PropertyType()>& read, const ScriptDelegate<void(const PropertyType&)>& write, FVoidEvent& onModifiedExternally) {\
		m_##PropertyName##Binding.read = read;\
		m_##PropertyName##Binding.write = write;\
		onModifiedExternally.Bind(FUNCTION_BINDING(this, Update_##PropertyName));\
		Update_##PropertyName();\
		return *this;\
	}

#define FUSION_PROPERTY_WRAPPER(PropertyName, WrappingVariable)\
	Self& PropertyName(const std::remove_cvref_t<decltype(std::declval<std::remove_cvref_t<decltype(*WrappingVariable)>>().PropertyName())>& value) {\
		if (WrappingVariable == nullptr) return *this;\
		WrappingVariable->PropertyName(value);\
		return *this;\
	}\
	auto PropertyName() const { return WrappingVariable->PropertyName(); }

#define FUSION_EVENT(EventType, PropertyName, ...)\
	protected:\
		EventType m_##PropertyName;\
	public:\
		auto& PropertyName() { return m_##PropertyName; }\
		Self& PropertyName(const FunctionBinding& binding)\
		{\
			m_##PropertyName.Bind(binding);\
			return *this;\
		}\
		template<typename TLambda>\
		Self& PropertyName(const TLambda& lambda)\
		{\
			m_##PropertyName.Bind(lambda);\
			return *this;\
		}\
		template<typename TLambda>\
		Self& PropertyName(DelegateHandle& outHandle, const TLambda& lambda)\
		{\
			outHandle = m_##PropertyName.Bind(lambda);\
			return *this;\
		}

#define FUSION_WIDGET\
	public:\
		template<typename TLambda> requires TValidate_IfTrue<TLambda>::Value\
		Self& IfTrue(bool condition, const TLambda& lambda)\
		{\
			if (condition) lambda(*this);\
			return *this;\
		}\
		FUSION_FRIENDS

#define BIND_PROPERTY_RW(modelPtr, propertyName)\
	FUNCTION_BINDING(model, Get##propertyName),\
	FUNCTION_BINDING(model, Set##propertyName##_UI),\
	modelPtr->On##propertyName##Modified()

#define BIND_PROPERTY_R(modelPtr, propertyName)\
	FUNCTION_BINDING(model, Get##propertyName),\
	nullptr,\
	modelPtr->On##propertyName##Modified()

#define MODEL_PROPERTY(PropertyType, PropertyName, ...) \
	protected:\
		PropertyType m_##PropertyName = {};\
		FUSION_EVENT(FVoidEvent, On##PropertyName##Modified)\
	public:\
		const auto& Get##PropertyName() const { return m_##PropertyName; }\
		void Set##PropertyName##_Raw(const PropertyType& value) { m_##PropertyName = value; }\
		void Set##PropertyName(const PropertyType& value) {\
			m_##PropertyName = value; m_On##PropertyName##Modified();\
			thread_local const CE::Name propName = #PropertyName;\
			THasOnModelPropertyModified<Self>::OnModelPropertyModified(this, propName);\
		}\

#define MODEL_PROPERTY_EDITABLE(PropertyType, PropertyName, ...) \
	protected:\
		PropertyType m_##PropertyName = {};\
		FUSION_EVENT(FVoidEvent, On##PropertyName##Modified)\
		FUSION_EVENT(FVoidEvent, On##PropertyName##Edited)\
	public:\
		const auto& Get##PropertyName() const { return m_##PropertyName; }\
		void Set##PropertyName##_Raw(const PropertyType& value) { m_##PropertyName = value; }\
		void Set##PropertyName(const PropertyType& value) {\
			m_##PropertyName = value; m_On##PropertyName##Modified();\
			thread_local const CE::Name propName = #PropertyName;\
			THasOnModelPropertyModified<Self>::OnModelPropertyModified(this, propName);\
		}\
		void Set##PropertyName##_UI(const PropertyType& value) {\
			m_##PropertyName = value; m_On##PropertyName##Edited();\
			thread_local const CE::Name propName = #PropertyName;\
			THasOnModelPropertyEdited<Self>::OnModelPropertyEdited(this, propName);\
		}\

namespace CE
{
	template<typename T, typename = void>
	struct THasOnModelPropertyModified : TFalseType
	{
		static void OnModelPropertyModified(T* instance, const CE::Name& propertyName) {} // Do nothing
	};

	template<typename T>
	struct THasOnModelPropertyModified<T, std::void_t<decltype(std::declval<T>().OnModelPropertyModified(CE::Name()))>> : TTrueType
	{
		static void OnModelPropertyModified(T* instance, const CE::Name& propertyName) { return instance->OnModelPropertyModified(propertyName); }
	};

	template<typename T, typename = void>
	struct THasOnModelPropertyEdited : TFalseType
	{
		static void OnModelPropertyEdited(T* instance, const CE::Name& propertyName) {} // Do nothing
	};

	template<typename T>
	struct THasOnModelPropertyEdited<T, std::void_t<decltype(std::declval<T>().OnModelPropertyEdited(CE::Name()))>> : TTrueType
	{
		static void OnModelPropertyEdited(T* instance, const CE::Name& propertyName) { return instance->OnModelPropertyEdited(propertyName); }
	};

	template<typename T>
	struct TGetReturnType
	{
		using Type = std::remove_cvref_t<typename TFunctionTraits<T>::ReturnType>;
	};

}
