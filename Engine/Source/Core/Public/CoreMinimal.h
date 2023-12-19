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
#include "Misc/Defer.h"
#include "Misc/Random.h"
#include "Memory/Memory.h"
#include "Memory/IAllocator.h"
#include "Memory/SystemAllocator.h"
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

// Module
#include "Module/Module.h"
#include "Module/ModuleManager.h"

// Delegates
#include "RTTI/RTTIDefines.h"
#include "Delegates/Delegate.h"
#include "Delegates/MultiCastDelegate.h"
#include "Delegates/DelegateRTTI.h"
#include "Misc/CoreDelegates.h"

// Threading
#include "Threading/Mutex.h"
#include "Threading/Thread.h"
#include "Threading/ThreadSingleton.h"
#include "Threading/Async.h"

// ******************************************************
// Math
#include "Math/Math.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/Color.h"
#include "Math/Gradient.h"

// Jobs
#include "Jobs/JobContext.h"
#include "Jobs/Job.h"
#include "Jobs/JobCompletion.h"
#include "Jobs/WorkQueue.h"
#include "Jobs/WorkThread.h"
#include "Jobs/JobManager.h"

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

// Json
#include "Json/Json.h"

// Structured Streams
#include "Serialization/StructuredStream.h"
#include "Serialization/Formatters/JsonStreamOutputFormatter.h"
#include "Serialization/Formatters/JsonStreamInputFormatter.h"


// RTTI & Object
#include "RTTI/RTTI.h"
#include "Object/Object.h"
#include "Object/Serialization.h"
#include "Object/FieldSerializer.h"
#include "Object/JsonSerializer.h"
#include "Object/Serialization/BinarySerializer.h"
#include "Object/ObjectGlobals.h"
#include "Object/ObjectManager.h"
#include "Object/SystemObject.h"
#include "Object/BasePrefs.h"
#include "Component/Component.h"
#include "Object/Package.h"

// Asset types
#include "Asset/SoftObjectPath.h"

// RTTI Dependents
#include "Serialization/BinaryBlob.h"
#include "Serialization/TextBlob.h"
#include "Math/MathRTTI.h"
#include "IO/FileWatcher.h"

// ******************************************************
// Messaging
#include "Messaging/MessageBus.h"
#include "Component/ComponentBus.h"
#include "Component/Component.h"
#include "Component/SystemComponent.h"
#include "Component/ComponentApplication.h"

