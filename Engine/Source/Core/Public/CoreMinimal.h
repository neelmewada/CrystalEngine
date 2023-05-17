#pragma once

// Basic headers
#include "CoreTypes.h"
#include "CoreTemplates.h"
#include "CoreContainers.h"

#include "CoreGlobals.h"

// ******************************************************
// Basic headers
#include "Misc/Exception.h"
#include "Misc/Assert.h"
#include "Misc/ByteUtils.h"
#include "Memory/Memory.h"
#include "Memory/FixedSizeAllocator.h"
#include "Logger/Logger.h"
#include "PAL/Common/PlatformMisc.h"
#include "PAL/Common/PlatformDirectories.h"
#include "PAL/Common/PlatformProcess.h"
#include "PAL/Common/PlatformTLS.h"

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
#include "Threading/ThreadSingleton.h"

// Module
#include "Module/Module.h"
#include "Module/ModuleManager.h"

// Delegates
#include "RTTI/RTTIDefines.h"
#include "Delegates/Delegate.h"
#include "Delegates/MultiCastDelegate.h"
#include "Misc/CoreDelegates.h"

// ******************************************************
// Math
#include "Math/Math.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/Color.h"

// Config INI
#include "Config/ConfigTypes.h"
#include "Config/ConfigLayers.h"
#include "Config/ConfigFile.h"

// ******************************************************
// RTTI, Object & Serialization

// Serialization
#include "Serialization/Stream.h"
#include "Serialization/MemoryStream.h"
#include "Serialization/FileStream.h"
#include "Serialization/ArchiveStream.h"

// Structured Streams
#include "Json/Json.h"
#include "Serialization/StructuredStream.h"
#include "Serialization/Formatters/JsonStreamOutputFormatter.h"
#include "Serialization/Formatters/JsonStreamInputFormatter.h"


// RTTI & Object
#include "RTTI/RTTI.h"
#include "Object/Object.h"
#include "Object/Serialization.h"
#include "Object/ObjectGlobals.h"
#include "Object/ObjectManager.h"
#include "Object/SystemObject.h"
#include "Object/Package.h"

// RTTI Dependents
#include "Math/MathRTTI.h"
#include "IO/FileWatcher.h"

// ******************************************************
// Messaging
#include "Messaging/MessageBus.h"
#include "Component/ComponentBus.h"
#include "Component/Component.h"
#include "Component/SystemComponent.h"
#include "Component/ComponentApplication.h"

