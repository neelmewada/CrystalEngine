
#include "CoreMinimal.h"

namespace CE
{
    void TlsAutoCleanup::RegisterForAutoCleanup()
    {
        class AutoCleanup
        {
        public:
            ~AutoCleanup()
            {
                for (auto target : targets)
                {
                    delete target;
                }

                targets.Clear();
            }

            void Add(TlsAutoCleanup* target)
            {
                targets.Add(target);
            }

            void Remove(TlsAutoCleanup* target)
            {
                targets.Remove(target);
            }

            u32 GetSize() const
            {
                return targets.GetSize();
            }
            
            Array<TlsAutoCleanup*> targets{};
        };
        
        static thread_local AutoCleanup cleanupTargets{};
        cleanupTargets.Add(this);
        auto size = cleanupTargets.targets.GetSize();

        for (int i = 0; i < cleanupTargets.targets.GetSize(); i++)
        {
            auto target = cleanupTargets.targets[i];
            size = cleanupTargets.targets.GetSize();
        }
    }

    TlsAutoCleanup* TlsInitializer::Get(std::function<TlsAutoCleanup*()> createInstance, u32& tlsSlot)
    {
        auto threadId = Thread::GetCurrentThreadId();
        
        if (tlsSlot == 0xFFFFFFFF)
        {
            tlsSlot = PlatformTLS::AllocTLS();
        }

        TlsAutoCleanup* threadSingleton = (TlsAutoCleanup*)PlatformTLS::GetTLSValue(tlsSlot);

        if (threadSingleton == nullptr)
        {
            threadSingleton = createInstance();
            threadSingleton->RegisterForAutoCleanup();
            PlatformTLS::SetTLSValue(tlsSlot, threadSingleton);
        }

        return threadSingleton;
    }
    
}
