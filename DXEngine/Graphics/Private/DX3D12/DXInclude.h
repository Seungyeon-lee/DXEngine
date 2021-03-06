#pragma once
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <wrl.h>
#include <system_error>
#include <D3Dcompiler.h>

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

constexpr inline uint32_t AlignGPUBufferSize(size_t size)
{
    constexpr uint32_t alignmentSize = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1;
    return (size + alignmentSize) & ~(alignmentSize);
}

constexpr inline uint32_t AlignTextureRowPitch(size_t size)
{
#if (defined(XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
    constexpr uint32_t alignmentSize = D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT - 1;
#else
    constexpr uint32_t alignmentSize = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1;
#endif
    return (size + alignmentSize) & ~(alignmentSize);
}
