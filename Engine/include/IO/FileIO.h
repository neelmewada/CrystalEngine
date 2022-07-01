#pragma once

#include "EngineDefs.h"

#include <string>
#include <vector>

namespace Vox::IO
{

bool ENGINE_API ReadAllBytesFromFile(const std::string& filePath, std::vector<char>& fileBuffer);

}
