
#include "IO/Path.h"


namespace CE::IO
{

	Path::Path() : impl()
	{
		
	}

	Path::~Path()
	{

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
		//if (begin() == end())
			return;

		auto it = begin();
		fs::path basePath{};

		if (it->string() == "@Engine")
		{
			basePath = (fs::path)PlatformDirectories::GetEngineDir();
		}
#if PAL_TRAIT_BUILD_EDITOR
		else if (it->string() == "@Editor")
		{
			basePath = (fs::path)PlatformDirectories::GetEditorDir();
		}
#endif
		else if (it->string() == "@Game")
		{
#if PAL_TRAIT_BUILD_EDITOR
			basePath = (fs::path)ProjectSettings::Get().GetEditorProjectDirectory() / "Game";
#else
			basePath = (fs::path)PlatformDirectories::GetGameDir();
#endif
		}
		else
		{
			return;
		}

		if (basePath.empty())
			return;

		fs::path evalPath{};
		evalPath = basePath;

		for (it = begin(); it != end(); ++it)
		{
			if (it == begin())
				continue;

			evalPath = evalPath / *it;
		}

		this->impl = evalPath;
	}

} // namespace CE::IO

