#pragma once

#define FNewOwned(WidgetClass, Parent)\
	(*CreateObject<WidgetClass>(Parent, #WidgetClass))

#define FNew(WidgetClass)\
	(*CreateObject<WidgetClass>(this, #WidgetClass))

#define FAssignNew(WidgetClass, VariableName) FNew(WidgetClass).Assign(VariableName)

#define FAssignNewOwned(WidgetClass, VariableName, Parent) FNewOwned(WidgetClass, Parent).Assign(VariableName)


#define FUSION_LAYOUT_PROPERTY(PropertyName)\
	Self& PropertyName(const decltype(m_##PropertyName)& value) { if (this->m_##PropertyName == value) return *this; this->m_##PropertyName = value; MarkLayoutDirty(); return *this; }\
	const auto& Get##PropertyName() const { return this->m_##PropertyName; }\
	void Set##PropertyName(const decltype(m_##PropertyName)& value) { \
		if (this->m_##PropertyName == value) return;\
		this->m_##PropertyName = value; static const CE::Name nameValue = #PropertyName; MarkLayoutDirty(); OnFusionPropertyModified(nameValue);\
	}

#define FUSION_PROPERTY(PropertyName)\
	Self& PropertyName(const decltype(m_##PropertyName)& value) { if (this->m_##PropertyName == value) return *this; this->m_##PropertyName = value; MarkDirty(); return *this; }\
	const auto& Get##PropertyName() const { return this->m_##PropertyName; }\
	void Set##PropertyName(const decltype(m_##PropertyName)& value) { \
		if (this->m_##PropertyName == value) return;\
		this->m_##PropertyName = value; static const CE::Name nameValue = #PropertyName; MarkDirty(); OnFusionPropertyModified(nameValue);\
	}

#define FUSION_WIDGET\
	public:\
		template<typename TLambda> requires TValidate_IfTrue<TLambda>::Value\
		Self& IfTrue(bool condition, const TLambda& lambda)\
		{\
			if (condition) lambda(*this);\
			return *this;\
		}
