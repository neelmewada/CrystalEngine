#pragma once

namespace CE
{
    
    struct IPropertyBinding
    {
        virtual ~IPropertyBinding() = default;

        virtual IScriptDelegate* GetWrite() = 0;
        virtual IScriptDelegate* GetRead() = 0;
        virtual bool IsBound() const = 0;
        virtual DelegateHandle GetModifyHandle() = 0;
    };

    template<typename T = CE::Variant>
    struct PropertyBinding : IPropertyBinding
    {
        PropertyBinding() {}

        virtual ~PropertyBinding() = default;

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

        DelegateHandle GetModifyHandle() override
        {
            return modifyDelegate;
        }
    };

} // namespace CE

CE_RTTI_POD_TEMPLATE(CORE_API, CE, PropertyBinding, CE::Variant)
