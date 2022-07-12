#pragma once


#define VK_ASSERT(result,message) if ((result) != VK_SUCCESS) throw std::runtime_error(message)
