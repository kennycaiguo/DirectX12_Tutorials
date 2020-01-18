#pragma once
#include <wrl.h>
#include <windows.h>
#include <windowsx.h>
#include <comdef.h>
#include <iostream>
#include <exception>
#include <cassert>
#include <stdio.h>
#include <string>
#include <vector>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include "d3dx12.h"

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName,
		const std::wstring& fileName, int lineNumber)
	{
		mErrorCode = hr;
		mFunctionName = functionName;
		mFileName = fileName;
		mLineNumber = lineNumber;
	}

	std::wstring ToString() const
	{
		_com_error err(mErrorCode);
		std::wstring msg = err.ErrorMessage();
		return mFunctionName + L" failed in " + mFileName + L"; line: "
			+ std::to_wstring(mLineNumber) + L"; error: " + msg;
	}

private:
	HRESULT mErrorCode = S_OK;
	std::wstring mFunctionName;
	std::wstring mFileName;
	int mLineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)						\
{												\
	HRESULT hr__ = (x);							\
	std::wstring wfn = AnsiToWString(__FILE__); \
	if (FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }\
}									
#endif

inline Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
	ID3D12Device* device, 
	ID3D12GraphicsCommandList* cmdList, 
	const void* initData,
	UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

	ThrowIfFailed(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, 
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize), 
			D3D12_RESOURCE_STATE_COMMON, nullptr, 
			IID_PPV_ARGS(defaultBuffer.GetAddressOf()))
	);

	ThrowIfFailed(
		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize), 
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 
			IID_PPV_ARGS(uploadBuffer.GetAddressOf()))
	);

	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = byteSize;

	cmdList->ResourceBarrier(1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST)
	);

	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	cmdList->ResourceBarrier(1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ)
	);

	return defaultBuffer;
}
