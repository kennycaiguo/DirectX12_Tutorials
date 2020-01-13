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
