#pragma once

#include "d3dUtil.h"
#include "UploadBuffer.h"


class FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(FrameResource const&) = delete;
	FrameResource& operator(FrameResource const&) = delete;
	~FrameResource() { }

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	std::unique_ptr<UploadBuffer<PassContants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	UINT Fence = 0;
};