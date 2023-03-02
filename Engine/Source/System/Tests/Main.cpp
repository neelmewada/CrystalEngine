
#include "CoreMinimal.h"
#include "System.h"

#include <iostream>



using namespace CE;

class SenderClass : public CE::Object
{
    CE_CLASS(SenderClass, CE::Object)
public:
    
    // Signals
    CE_SIGNAL(OnTextEdited, CE::String, CE::f32);
};

CE_RTTI_CLASS(,, SenderClass,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        // Signals
        CE_FUNCTION(OnTextEdited, Signal)
    )
)

CE_RTTI_CLASS_IMPL(,, SenderClass)

class ReceiverClass : public CE::Object
{
    CE_CLASS(ReceiverClass, CE::Object)
public:
    void OnInputFieldEdited(CE::String string, CE::f32 f)
    {
        CE_LOG(Info, All, "Value changed: {} , {}", string, f);
    }
};

CE_RTTI_CLASS(,, ReceiverClass,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(OnInputFieldEdited, Event)
    )
)

CE_RTTI_CLASS_IMPL(,, ReceiverClass)

int main(int argc, char** argv)
{
	using namespace CE;
    
	CE::Logger::Initialize();
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("System");
    
    CE_REGISTER_TYPES(SenderClass, ReceiverClass);
    
    SenderClass* s = new SenderClass;
    
    ReceiverClass* r = new ReceiverClass;
    
    auto result = s->Bind("OnTextEdited", r, "OnInputFieldEdited");
    
    s->OnTextEdited("Hello", 12.4124f);
    
    delete r;
    
    s->OnTextEdited("Hello", 12.4124f);
    
    delete s;
    
	CE::ModuleManager::Get().UnloadModule("System");
	CE::ModuleManager::Get().UnloadModule("Core");
	CE::Logger::Shutdown();
    
    return 0;
}
