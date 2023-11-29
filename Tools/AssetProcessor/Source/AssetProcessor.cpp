#include "AssetProcessor.h"

#include "cxxopts.hpp"

AssetProcessor::AssetProcessor(int argc, char** argv)
{
	cxxopts::Options options("Asset Processor", "A command line tool to process assets.");

	options.add_options()
		("h,help", "Print this help info.")
		("p,pack", "Package the assets for final distribution build. (Not implemented yet)")
		;
}

AssetProcessor::~AssetProcessor()
{
}

int AssetProcessor::Run()
{
	return 0;
}

void AssetProcessor::Initialize()
{
}

void AssetProcessor::Shutdown()
{
}
