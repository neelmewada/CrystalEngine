#pragma once

namespace CE
{
    class Object;

    struct IObjectUpdateListener
    {
        virtual ~IObjectUpdateListener() {}

        virtual void OnObjectFieldChanged(Object* object, const Name& fieldName) {}
    };

    class ObjectListener
    {
        CE_STATIC_CLASS(ObjectListener)
    public:

        static void AddListener(IObjectUpdateListener* listener);

        static void RemoveListener(IObjectUpdateListener* listener);

    private:

        static void Trigger(Object* object, const Name& fieldName);

        static SharedMutex mutex;
        static HashSet<IObjectUpdateListener*> listeners;

        friend class Object;
    };

} // namespace CE
