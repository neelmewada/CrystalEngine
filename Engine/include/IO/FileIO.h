#pragma once

#include <string>
#include <vector>

namespace Vox::IO
{

bool ReadAllBytesFromFile(const std::string& filePath, std::vector<char>& fileBuffer);

}
