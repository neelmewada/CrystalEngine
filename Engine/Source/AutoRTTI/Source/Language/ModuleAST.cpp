
#include "ModuleAST.h"

namespace CE
{

	ModuleAST::ModuleAST(String moduleName)
		: moduleName(moduleName)
	{

	}

	ModuleAST::~ModuleAST()
	{
		for (auto header : processedHeaders)
		{
			delete header;
		}
		processedHeaders.Clear();
	}

	void ModuleAST::ProcessHeader(IO::Path headerPath, const Array<IO::Path>& includeSearchPaths, std::stringstream& outStream)
	{
		auto tokens = HeaderTokenizer::Tokenize(headerPath, includeSearchPaths);

        outStream << "#pragma once\n\n";
        
        auto ast = HeaderAST::ProcessHeader(tokens);
        
        delete tokens;
        delete ast;
	}

} // namespace CE
