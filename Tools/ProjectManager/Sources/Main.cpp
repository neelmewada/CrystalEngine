
#include "CoreMinimal.h"
#include "System.h"

#include "cxxopts.hpp"
#include <iostream>

#define PRINT(x) std::cout << x << std::endl
#define ERROR(x) std::cerr << x << std::endl

int main(int argc, char** argv)
{
    cxxopts::Options options("ProjectManager", "A command line tool to create & edit Crystal Engine projects");

    options.add_options()
        ("h,help", "Print this help info.")
        ("c,create", "Create a new project.")
        ("t,template", "Create project from a specified template. Leave empty to create a blank project.", cxxopts::value<std::string>()->default_value(""))
        ;

    try
    {
        cxxopts::ParseResult result = options.parse(argc, argv);

         if (result["h"].as<bool>())
         {
             PRINT(options.help());
             return 0;
         }

    }
    catch (std::exception exc)
    {
        ERROR("Error: " << exc.what());
    }

    return 0;
}