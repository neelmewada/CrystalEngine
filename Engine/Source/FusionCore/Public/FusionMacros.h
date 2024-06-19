#pragma once

#define FNewOwned(WidgetClass, Parent)\
	(*CreateObject<WidgetClass>(Parent, #WidgetClass))

#define FNew(WidgetClass)\
	(*CreateObject<WidgetClass>(this, #WidgetClass))

#define FAssignNew(WidgetClass, VariableName) FNew(WidgetClass).Assign(VariableName)

#define FAssignNewOwned(WidgetClass, VariableName, Parent) FNewOwned(WidgetClass, Parent).Assign(VariableName)


#define FUSION_LAYOUT_PROPERTY(PropertyName)\
	Self& PropertyName(const decltype(m_##PropertyName)& value) {\
		if (this->m_##PropertyName == value) return *this; this->m_##PropertyName = value; MarkLayoutDirty();\
		static const CE::Name nameValue = #PropertyName;\
		OnFusionPropertyModified(nameValue);\
		return *this;\
	}\
	const auto& Get##PropertyName() const { return this->m_##PropertyName; }

#define FUSION_PROPERTY(PropertyName)\
	Self& PropertyName(const decltype(m_##PropertyName)& value) {\
		if (this->m_##PropertyName == value)\
			return *this;\
		this->m_##PropertyName = value; MarkDirty();\
		static const CE::Name nameValue = #PropertyName;\
		OnFusionPropertyModified(nameValue);\
		return *this;\
	}\
	const auto& Get##PropertyName() const { return this->m_##PropertyName; }

#define FUSION_EVENT(PropertyName)\
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
		}
