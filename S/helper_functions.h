#ifndef HELPER_FUNCIONS_H_INCLUDED
#define HELPER_FUNCIONS_H_INCLUDED

#include <Windows.h>
#include <exception>
#include <string>
#include <d3dcompiler.h>
#include <assert.h>

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw std::exception();
}

inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
{
	using namespace Microsoft::WRL;

	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

	Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
	if (file.Get() == INVALID_HANDLE_VALUE)
	{
		throw std::exception();
	}

	FILE_STANDARD_INFO fileInfo = {};
	if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
	{
		throw std::exception();
	}

	if (fileInfo.EndOfFile.HighPart != 0)
	{
		throw std::exception();
	}

	*data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
	*size = fileInfo.EndOfFile.LowPart;

	if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
	{
		throw std::exception();
	}

	return S_OK;
}

#if defined(_DEBUG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT const index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName(x.Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed(x[n].Get(), L#x, n)

static inline UINT default_compile_flags()
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT const compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT const compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	return compileFlags;
}

static inline D3D12_CLEAR_VALUE clear_value(DXGI_FORMAT format, D3D12_DEPTH_STENCIL_VALUE depth_stencil)
{
	D3D12_CLEAR_VALUE result{};
	result.Format = format;
	result.DepthStencil = depth_stencil;
	return result;
}

static inline D3D12_CLEAR_VALUE clear_value(DXGI_FORMAT format, std::initializer_list<FLOAT> color)
{
	assert(color.size() == 4);

	D3D12_CLEAR_VALUE result{};
	result.Format = format;
	int i = 0;
	for (auto const& c : color)
		result.Color[i++] = c;
	return result;
}

#endif // #ifndef HELPER_FUNCIONS_H_INCLUDED