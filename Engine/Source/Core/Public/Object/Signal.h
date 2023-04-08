#pragma once

namespace CE
{
    class StructType;
    class ClassType;
    class Object;

    struct SignalBinding
    {
    public:
        FunctionType* signalFuntion;
        FunctionType* boundFunction;
        Object* boundObject;
    };

} // namespace CE

#define __CE_ARG_0(...)
#define __CE_ARG_1(Type, ...) Type a0
#define __CE_ARG_2(Type, ...) Type a1, CE_EXPAND(__CE_ARG_1(__VA_ARGS__))
#define __CE_ARG_3(Type, ...) Type a2, CE_EXPAND(__CE_ARG_2(__VA_ARGS__))
#define __CE_ARG_4(Type, ...) Type a3, CE_EXPAND(__CE_ARG_3(__VA_ARGS__))
#define __CE_ARG_5(Type, ...) Type a4, CE_EXPAND(__CE_ARG_4(__VA_ARGS__))
#define __CE_ARG_6(Type, ...) Type a5, CE_EXPAND(__CE_ARG_5(__VA_ARGS__))
#define __CE_ARG_7(Type, ...) Type a6, CE_EXPAND(__CE_ARG_6(__VA_ARGS__))
#define __CE_ARG_8(Type, ...) Type a7, CE_EXPAND(__CE_ARG_7(__VA_ARGS__))
#define __CE_ARG_9(Type, ...) Type a8, CE_EXPAND(__CE_ARG_8(__VA_ARGS__))

#define __CE_ARG_LIST_0
#define __CE_ARG_LIST_1 a0
#define __CE_ARG_LIST_2 a1, __CE_ARG_LIST_1
#define __CE_ARG_LIST_3 a2, __CE_ARG_LIST_2
#define __CE_ARG_LIST_4 a3, __CE_ARG_LIST_3
#define __CE_ARG_LIST_5 a4, __CE_ARG_LIST_4
#define __CE_ARG_LIST_6 a5, __CE_ARG_LIST_5
#define __CE_ARG_LIST_7 a6, __CE_ARG_LIST_6
#define __CE_ARG_LIST_8 a7, __CE_ARG_LIST_7
#define __CE_ARG_LIST_9 a8, __CE_ARG_LIST_8

#define CE_SIGNAL(SignalName, ...) inline void SignalName(CE_EXPAND(CE_EXPAND(CE_CONCATENATE(__CE_ARG_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)))\
{\
    CE::Array<CE::Variant> args = { CE_EXPAND(CE_CONCATENATE(__CE_ARG_LIST_,CE_ARG_COUNT(__VA_ARGS__))) };\
    FireSignal(#SignalName, args);\
}


