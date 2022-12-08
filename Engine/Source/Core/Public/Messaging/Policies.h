#pragma once


namespace CE
{

    enum class MBusAddressPolicy
    {
        /// (Default) Message bus supports single address, i.e. No addressing used
        Single,

        /// Message bus supports addressing via multiple IDs
        ById,

        /// Message bus supports addressing via multiple IDs, and the execution order is in the increasing order of their IDs
        ByIdAndOrdered,
    };

    enum class MBusHandlerPolicy
    {
        /// Message bus supports single handler
        Single,

        /// (Default) Message bus supports multiple handlers
        Multiple,

        /// Message bus supports multiple orders, and the execution order is in the increasing order of their GetHandlerOrder() overrides
        MultipleAndOrdered
    };
    
    
} // namespace CE

