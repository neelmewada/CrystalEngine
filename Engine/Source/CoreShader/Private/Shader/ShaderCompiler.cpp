
#include "CoreShader.h"

#include <PAL/Common/PlatformSystemIncludes.h>

#if PLATFORM_WINDOWS
#else
#   define __EMULATE_UUID
#endif

#include <dxc/dxcapi.h>

#include <locale>
#include <codecvt>
#include <string>

namespace CE
{
	static std::wstring ToWString(const std::string& stringToConvert)
	{
		std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}

	static std::wstring ToWString(const String& string)
	{
		std::string stringToConvert = string.ToStdString();
		std::wstring wideString =
			std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}
	
	struct ShaderCompiler::Impl
	{
		CComPtr<IDxcUtils> utils;
		CComPtr<IDxcCompiler3> compiler;
		CComPtr<IDxcIncludeHandler> includeHandler;

		~Impl()
		{
			utils.Release();
			compiler.Release();
			includeHandler.Release();
		}
	};

	ShaderCompiler::ShaderCompiler()
	{
		impl = new Impl();

		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&impl->utils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&impl->compiler));

		// Create default include handler
		impl->utils->CreateDefaultIncludeHandler(&impl->includeHandler);
	}

	ShaderCompiler::~ShaderCompiler()
	{
		delete impl;
	}

	ShaderCompiler::ErrorCode ShaderCompiler::BuildSpirv(const IO::Path& hlslPath, const ShaderBuildConfig& buildConfig, void** outByteCode, u32* outByteCodeSize, Array<std::wstring>& extraArgs)
	{
		if (!hlslPath.Exists())
			return ERR_FileNotFound;
		if (hlslPath.IsDirectory())
			return ERR_InvalidFile;

		HRESULT status;
		ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

		CComPtr<IDxcBlobEncoding> source = nullptr;
		std::wstring str = ToWString(hlslPath.GetString());
		std::wstring fileNameStr = ToWString(hlslPath.GetFilename().GetString());

		status = impl->utils->LoadFile(str.data(), nullptr, &source);
		
		if (!SUCCEEDED(status))
		{
			this->errorMessage = "Failed to load source file";
			return ERR_FailedToLoadFile;
		}

		DxcBuffer buffer;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;

		Array<const wchar_t*> wcharArgs{};
		for (const auto& arg : extraArgs)
		{
			wcharArgs.Add(arg.data());
		}

		wcharArgs.AddRange({
			fileNameStr.data()
		});

		Array<std::wstring> includeSearchPathsWString = config.includeSearchPaths.Transform<std::wstring>([&](IO::Path& path)
			{
				return ToWString(path.GetString());
			});

		for (const auto includePath : includeSearchPathsWString)
		{
			wcharArgs.AddRange({ L"-I", includePath.data() });
		}

		std::wstring entryName = ToWString(buildConfig.entry);

		wcharArgs.AddRange({ L"-E", entryName.data() });

		if (buildConfig.stage == ShaderStage::Vertex)
		{
			wcharArgs.AddRange({ L"-T", L"vs_6_0" });
		} 
		else if (buildConfig.stage == ShaderStage::Fragment)
		{
			wcharArgs.AddRange({ L"-T", L"ps_6_0" });
		}
		else
		{
			return ERR_InvalidArgs;
		}

		Array<std::wstring> globalDefinesWString = config.globalDefines.Transform<std::wstring>([&](String& string)
			{
				return ToWString(string);
			});

		for (const auto& define : globalDefinesWString)
		{
			wcharArgs.AddRange({ L"-D", define.data() });
		}

		CComPtr<IDxcResult> results;
		status = impl->compiler->Compile(
			&buffer,                // Source buffer.
			wcharArgs.GetData(),                // Array of pointers to arguments.
			wcharArgs.GetSize(),      // Number of arguments.
			impl->includeHandler,        // User-provided interface to handle #include directives (optional).
			IID_PPV_ARGS(&results) // Compiler output status, buffer, and errors.
		);

		results->GetStatus(&status);

		if (!SUCCEEDED(status))
		{
			CComPtr<IDxcBlobEncoding> error;
			results->GetErrorBuffer(&error);
			char* message = (char*)error->GetBufferPointer();
			auto size = error->GetBufferSize();
			this->errorMessage = message;
			return ERR_CompilationFailure;
		}

		CComPtr<IDxcBlob> blob;
		status = results->GetResult(&blob);

		if (!SUCCEEDED(status))
		{
			return ERR_CompilationFailure;
		}

		*outByteCodeSize = blob->GetBufferSize();
		*outByteCode = Memory::Malloc(*outByteCodeSize);
		memcpy(*outByteCode, blob->GetBufferPointer(), *outByteCodeSize);

		return ErrorCode::ERR_Success;
	}

	ShaderCompiler::ErrorCode ShaderCompiler::BuildSpirv(const void* data, u32 dataSize, const ShaderBuildConfig& buildConfig, 
		void** outByteCode, u32* outByteCodeSize, Array<std::wstring>& extraArgs)
	{
		HRESULT status;
		ShaderBuildConfig& config = const_cast<ShaderBuildConfig&>(buildConfig);

		CComPtr<IDxcBlobEncoding> source = nullptr;

		status = impl->utils->CreateBlob(data, dataSize, DXC_CP_UTF8, &source);

		if (!SUCCEEDED(status))
		{
			this->errorMessage = "Failed to load source file";
			return ERR_FailedToLoadFile;
		}

		DxcBuffer buffer;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;

		Array<const wchar_t*> wcharArgs{};
		for (const auto& arg : extraArgs)
		{
			wcharArgs.Add(arg.data());
		}

		//wcharArgs.AddRange({
		//	fileNameStr.data()
		//});

		Array<std::wstring> includeSearchPathsWString = config.includeSearchPaths.Transform<std::wstring>([&](IO::Path& path)
			{
				return ToWString(path.GetString());
			});

		for (const auto includePath : includeSearchPathsWString)
		{
			wcharArgs.AddRange({ L"-I", includePath.data() });
		}

		std::wstring entryName = ToWString(buildConfig.entry);

		wcharArgs.AddRange({ L"-E", entryName.data() });

		if (buildConfig.stage == ShaderStage::Vertex)
		{
			wcharArgs.AddRange({ L"-T", L"vs_6_0" });
		}
		else if (buildConfig.stage == ShaderStage::Fragment)
		{
			wcharArgs.AddRange({ L"-T", L"ps_6_0" });
		}
		else
		{
			return ERR_InvalidArgs;
		}

		Array<std::wstring> globalDefinesWString = config.globalDefines.Transform<std::wstring>([&](String& string)
			{
				return ToWString(string);
			});

		for (const auto& define : globalDefinesWString)
		{
			wcharArgs.AddRange({ L"-D", define.data() });
		}

		CComPtr<IDxcResult> results;
		status = impl->compiler->Compile(
			&buffer,                // Source buffer.
			wcharArgs.GetData(),                // Array of pointers to arguments.
			wcharArgs.GetSize(),      // Number of arguments.
			impl->includeHandler,        // User-provided interface to handle #include directives (optional).
			IID_PPV_ARGS(&results) // Compiler output status, buffer, and errors.
		);

		defer(
			results.Release();
		);

		results->GetStatus(&status);

		if (!SUCCEEDED(status))
		{
			CComPtr<IDxcBlobEncoding> error;
			results->GetErrorBuffer(&error);
			char* message = (char*)error->GetBufferPointer();
			auto size = error->GetBufferSize();
			this->errorMessage = message;
			return ERR_CompilationFailure;
		}

		CComPtr<IDxcBlob> blob;
		status = results->GetResult(&blob);

		if (!SUCCEEDED(status))
		{
			return ERR_CompilationFailure;
		}

		*outByteCodeSize = blob->GetBufferSize();
		*outByteCode = Memory::Malloc(*outByteCodeSize);
		memcpy(*outByteCode, blob->GetBufferPointer(), *outByteCodeSize);

		return ErrorCode::ERR_Success;
	}

	ShaderCompiler::ErrorCode ShaderCompiler::Compile(const IO::Path& filePath, Array<std::wstring>& args, void** outByteCode, u32* outByteCodeSize)
	{
		if (!filePath.Exists())
			return ERR_FileNotFound;
		if (filePath.IsDirectory())
			return ERR_InvalidFile;

		HRESULT status;

		CComPtr<IDxcBlobEncoding> source = nullptr;
		std::wstring str = ToWString(filePath.GetString());
		std::wstring fileNameStr = ToWString(filePath.GetFilename().GetString());

		status = impl->utils->LoadFile(str.data(), nullptr, &source);
		if (!SUCCEEDED(status))
		{
			this->errorMessage = "Failed to load source file";
			return ERR_FailedToLoadFile;
		}

		DxcBuffer buffer;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();
		buffer.Encoding = DXC_CP_UTF8;//DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.

		Array<const wchar_t*> wcharArgs{};
		for (const auto& arg : args)
		{
			wcharArgs.Add(arg.data());
		}

		wcharArgs.AddRange({
			fileNameStr.data()
		});

		CComPtr<IDxcResult> results;
		status = impl->compiler->Compile(
			&buffer,                // Source buffer.
			wcharArgs.GetData(),                // Array of pointers to arguments.
			wcharArgs.GetSize(),      // Number of arguments.
			impl->includeHandler,        // User-provided interface to handle #include directives (optional).
			IID_PPV_ARGS(&results) // Compiler output status, buffer, and errors.
		);

		results->GetStatus(&status);

		if (!SUCCEEDED(status))
		{
			CComPtr<IDxcBlobEncoding> error;
			results->GetErrorBuffer(&error);
			char* message = (char*)error->GetBufferPointer();
			auto size = error->GetBufferSize();
			this->errorMessage = message;
			return ERR_CompilationFailure;
		}

		CComPtr<IDxcBlob> blob;
		status = results->GetResult(&blob);

		if (!SUCCEEDED(status))
		{
			return ERR_CompilationFailure;
		}

		*outByteCodeSize = blob->GetBufferSize();
		*outByteCode = Memory::Malloc(*outByteCodeSize);
		memcpy(*outByteCode, blob->GetBufferPointer(), *outByteCodeSize);

		return ERR_Success;
	}

} // namespace CE::Editor

