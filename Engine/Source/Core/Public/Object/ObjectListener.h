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

        static void AddListener(Object* target, IObjectUpdateListener* listener);

        static void RemoveListener(Object* target, IObjectUpdateListener* listener);
        static void RemoveAllListeners(Object* target);

    private:

        static void Trigger(Object* object, const Name& fieldName);

        static SharedMutex mutex;
        static HashMap<Object*, Array<IObjectUpdateListener*>> listeners;

        friend class Object;
    };

} // namespace CE
