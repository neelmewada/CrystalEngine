#pragma once


namespace CE
{

    enum class MBusAddressPolicy
    {
        /// (Default) Message bus supports single address, i.e. No addressing used
        Single,

        /// Message bus supports addressing via multiple Ids
        ById,
    };

    enum class MBusHandlerPolicy
    {
        /// Message bus supports single handler
        Single,

        /// (Default) Message bus supports multiple handlers
        Multiple
    };
    
    
} // namespace CE

