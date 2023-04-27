#pragma once

// Basic headers
#include "CoreTypes.h"

// ******************************************************
// Basic headers
#include "Memory/Memory.h"
#include "Memory/FixedSizeAllocator.h"
#include "Logger/Logger.h"
#include "PAL/Common/PlatformMisc.h"
#include "PAL/Common/PlatformDirectories.h"
#include "PAL/Common/PlatformProcess.h"
// IO
#include "IO/Path.h"
#include "IO/Stream/GenericStream.h"
#include "IO/Stream/MemoryStream.h"
#include "IO/Stream/FileStream.h"
#include "IO/Archive.h"

// *********************************

// Threading
#include "Threading/Mutex.h"
#include "Threading/Thread.h"

// Module
#include "Module/Module.h"
#include "Module/ModuleManager.h"


// Delegates
#include "RTTI/RTTIDefines.h"
#include "Delegates/Delegate.h"
#include "Delegates/MultiCastDelegate.h"
#include "Delegates/CoreDelegates.h"

// ******************************************************
// RTTI, Object & Serialization

#include "RTTI/RTTI.h"
#include "Object/Object.h"
#include "Object/Package.h"
#include "Object/ObjectGlobals.h"
#include "Object/ObjectManager.h"
#include "Object/Serialization.h"
#include "Object/SystemObject.h"

// IO::FileWatcher
#include "IO/FileWatcher.h"

// *****************************************************
// Project
#include "Project/ProjectSettings.h"

// ******************************************************
// Math
#include "Math/Math.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/Color.h"
#include "Math/MathRTTI.h"

// ******************************************************
// Messaging
#include "Messaging/MessageBus.h"
#include "Component/ComponentBus.h"
#include "Component/Component.h"
#include "Component/SystemComponent.h"
#include "Component/ComponentApplication.h"

