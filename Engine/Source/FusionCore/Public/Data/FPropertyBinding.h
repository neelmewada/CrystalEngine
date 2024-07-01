#pragma once

#define BIND_PROPERTY_RW(modelPtr, propertyName)\
	CE::MemberDelegate(&std::remove_cvref_t<decltype(*model)>::Get##propertyName, modelPtr), \
	CE::MemberDelegate(&std::remove_cvref_t<decltype(*model)>::Set##propertyName##_UI, modelPtr), \
	modelPtr->On##propertyName##Modified()

#define BIND_PROPERTY_R(modelPtr, propertyName)\
	CE::MemberDelegate(&std::remove_cvref_t<decltype(*model)>::Get##propertyName, modelPtr), \
	nullptr, \
	modelPtr->On##propertyName##Modified()

namespace CE
{

    struct IPropertyBinding
    {
        virtual ~IPropertyBinding() = default;

        virtual IScriptDelegate* GetWrite() = 0;
        virtual IScriptDelegate* GetRead() = 0;
        virtual bool IsBound() const = 0;
    };

    template<typename T>
    struct FPropertyBinding : IPropertyBinding
    {
        FPropertyBinding() {}

        ScriptDelegate<void(const T&)> write;
        ScriptDelegate<T()> read;
        DelegateHandle modifyDelegate = 0;

        bool IsBound() const override
        {
            return read.IsBound();
        }

        IScriptDelegate* GetWrite() override
        {
            return &write;
        }

        IScriptDelegate* GetRead() override
        {
            return &read;
        }
    };
    
} // namespace CE
