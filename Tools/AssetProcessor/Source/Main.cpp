
#include "AssetProcessor.h"

#include "AssetProcessor.private.h"

int main(int argc, char** argv)
{
	AssetProcessor assetProcessor{ argc, argv };

	return assetProcessor.Run();
}
