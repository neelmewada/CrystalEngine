
#include "IO/FileIO.h"

#include <iostream>
#include <fstream>

namespace Vox::IO
{

bool Vox::IO::ReadAllBytesFromFile(const std::string &filePath, std::vector<char> &fileBuffer)
{
    // ios::ate tells file to start pointer at the end instead of start
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file at: " << filePath << std::endl;
        return false;
    }

    size_t fileSize = (size_t) file.tellg();
    fileBuffer.resize(fileSize);
    file.seekg(0); // Move read position pointer to start of the file

    file.read(fileBuffer.data(), fileSize);
    file.close();

    return true;
}


}