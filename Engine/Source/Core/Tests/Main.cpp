
#include "CoreMinimal.h"

#include <iostream>

#define LOG(x) std::cout << x << std::endl

void PassString(CE::String pass)
{
    LOG("Passed: " << pass);
}

int main(int argc, char* argv[])
{
    CE::Array<float> arr = { 12, 53.12f };

    for (auto item : arr)
    {
        LOG("Item = " << item);
    }


    return 0;
}

