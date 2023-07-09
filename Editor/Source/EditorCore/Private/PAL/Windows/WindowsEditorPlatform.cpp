#include "EditorCore.h"

#include <Windows.h>

#include <ShObjIdl_core.h>

#include <locale>
#include <codecvt>
#include <string>

namespace CE::Editor
{
	static std::wstring ToWString(const std::string& stringToConvert)
	{
		std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}

	static std::wstring ToWString(const String& string)
	{
		std::string stringToConvert = string.ToStdString();
		std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);

		return wideString;
	}

	static std::string ToString(const std::wstring& wideString)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX{};

		return converterX.to_bytes(wideString);
	}

	IO::Path WindowsEditorPlatform::ShowSelectDirectoryDialog(const IO::Path& defaultPath)
	{
		IO::Path result{};

		IFileDialog* pfd = nullptr;
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
		{
			DWORD dwOptions;
			if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
			{
				pfd->SetOptions(FOS_PICKFOLDERS);
			}

			if (SUCCEEDED(pfd->Show(NULL)))
			{
				IShellItem* psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					LPWSTR pathString{};
					if (SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pathString)))
					{
						std::wstring wideString = std::wstring(pathString);
						result = ToString(wideString);
					}
					psi->Release();
				}
			}
			pfd->Release();
		}

		return result;
	}

} // namespace CE::Editor
