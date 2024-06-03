#pragma once

namespace CE
{
	namespace Internal
	{
        struct ObjectCreateParams;
	}

    class ObjectCreationStack;
    using ObjectCreationContext = ThreadLocalContext<ObjectCreationStack>;

    class CORE_API ObjectCreationStack
    {
    public:

        ObjectCreationStack();

        virtual ~ObjectCreationStack();

        void Push(Internal::ObjectCreateParams* params)
        {
            parameterStack.Add(params);
        }

        void Pop()
        {
            if (!parameterStack.IsEmpty())
                parameterStack.RemoveAt(parameterStack.GetSize() - 1);
        }

        Internal::ObjectCreateParams* Top()
        {
            return parameterStack.IsEmpty() ? nullptr : parameterStack.GetLast();
        }

    public:

        friend class UniquePtr<ObjectCreationStack>;
        friend class ThreadLocalContext<ObjectCreationStack>;

        Array<Internal::ObjectCreateParams*> parameterStack{};
    };

    CORE_API ObjectCreationContext* GetObjectCreationContext();

} // namespace CE
