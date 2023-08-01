
#include "IO/Path.h"


namespace CE::IO
{

	Path::Path() : impl()
	{
		
	}

	Path::~Path()
	{

	}

    bool Path::IsSubDirectory(Path path, Path root)
    {
		for (auto b = root.begin(), s = path.begin(); b != root.end(); ++b, ++s)
		{
			if (s == path.end() || *s != *b)
			{
				return false;
			}
		}
		return true;
    }

    void Path::IterateChildren(std::function<void(const IO::Path& path)> func)
	{
		for (const auto& iter : fs::directory_iterator(impl))
		{
			func(iter.path());
		}
	}

	void Path::RecursivelyIterateChildren(std::function<void(const IO::Path& path)> func)
	{
		for (const auto& iter : fs::recursive_directory_iterator(impl))
		{
			func(iter.path());
		}
	}

	void Path::EvaluateVirtualPath()
	{
		
	}

} // namespace CE::IO

