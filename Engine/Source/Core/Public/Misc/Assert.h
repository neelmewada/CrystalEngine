#pragma once

#include "Misc/Exception.h"

#include <cassert>

#define CE_ASSERT(Condition, Message, ...)\
if (!(Condition)) { CE_LOG(Critical, All, Message "", ##__VA_ARGS__); assert(false); }

