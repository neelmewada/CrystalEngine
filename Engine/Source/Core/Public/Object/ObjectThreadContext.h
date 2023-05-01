#pragma once

namespace CE
{
    class ObjectInitializer;

    class CORE_API ObjectThreadContext : public ThreadSingleton<ObjectThreadContext>
    {
        ObjectThreadContext();

        virtual ~ObjectThreadContext();

    public:

        void PushInitializer(ObjectInitializer* initializer)
        {
            objectInitializerStack.Add(initializer);
        }

        void PopInitializer()
        {
            if (!objectInitializerStack.IsEmpty())
                objectInitializerStack.RemoveAt(objectInitializerStack.GetSize() - 1);
        }

        ObjectInitializer* TopInitializer()
        {
            return objectInitializerStack.IsEmpty() ? nullptr : objectInitializerStack.GetLast();
        }

    public:// TODO: Change to private after finish debugging
        friend class ThreadSingleton<ObjectThreadContext>;

        Array<ObjectInitializer*> objectInitializerStack{};
    };

} // namespace CE
