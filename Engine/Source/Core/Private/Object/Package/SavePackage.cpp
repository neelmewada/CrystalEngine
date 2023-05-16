
#include "CoreMinimal.h"

#define CPAK_VERSION_MAJOR (u16)0
#define CPAK_VERSION_MINOR (u16)1

#define CPAK_MAGIC_NUMBER 0x004350414b00000a // .CPAK..\n

#define CPACK_OBJECT_MAGIC_NUMBER 0x004f424a45435400 // .OBJECT.

namespace CE
{

    SavePackageResult Package::SavePackage(Package *outer,
                                           Object *object,
                                           const String& fileName,
                                           const SavePackageArgs& args)
    {
        return SavePackageResult::Success;
    }

} // namespace CE

