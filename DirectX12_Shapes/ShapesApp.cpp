#include "ShapesApp.h"
#include "GeometryGenerator.h"

using namespace DirectX;

const int gNumFrameResources = 3;

void ShapesApp::Update()
{
	mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
	mCurrentFrameResource = mFrameResources[mCurrentFrameResourceIndex].get();

	if (mCurrentFrameResource->Fence != 0 &&
		mFence->GetCompletedValue() < mCurrentFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(
			mFence->SetEventOnCompletion(mCurrentFrameResource->Fence, eventHandle)
		);
		WaitForSingleObject(eventHandle, INFINITE);
	}
}

void ShapesApp::Draw()
{
	// ... 构建和提交本帧的命令列表

	mCurrentFrameResource->Fence = ++mCurrentFence;
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void ShapesApp::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox();
	GeometryGenerator::MeshData grid = geoGen.CreateGrid();
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere();
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + sphere.Vertices.size();

	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT) box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;
	
	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT) sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	auto totalVertexCount = box.Vertices.size() + grid.Vertices.size() 
		+ sphere.Vertices.size() + cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);
	UINT k = 0;

	for (std::size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (std::size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (std::size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (std::size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";
	
	ThrowIfFailed(
		D3DCreateBlob(vbByteSize, &geo->VertexBuffer)
	);
	CopyMemory(geo->VertexBuffer->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(
		D3DCreateBlob(ibByteSize, &geo->IndexBuffer)
	);
	CopyMemory(geo->IndexBuffer->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(),
		vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(),
		indices.data(), ibByteSize, geo->IndexBufferUploader);
	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void ShapesApp::BuildRenderItems()
{
	auto boxRenderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRenderItem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRenderItem->ObjCBIndex = 0;
	boxRenderItem->Geo = mGeometries["shapeGeo"].get();
	boxRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRenderItem->IndexCount = boxRenderItem->Geo->DrawArgs["box"].IndexCount;
	boxRenderItem->StartIndexLocation = boxRenderItem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRenderItem->BaseVertexLocation = boxRenderItem->Geo->DrawArgs["box"].BaseVertexLocation;
	mAllRenderItems.push_back(std::move(boxRenderItem));

	auto gridRenderItem = std::make_unique<RenderItem>();
	gridRenderItem->ObjCBIndex = 1;
	gridRenderItem->Geo = mGeometries["shapeGeo"].get();
	gridRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRenderItem->IndexCount = gridRenderItem->Geo->DrawArgs["grid"].IndexCount;
	gridRenderItem->StartIndexLocation = gridRenderItem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRenderItem->BaseVertexLocation = gridRenderItem->Geo->DrawArgs["grid"].BaseVertexLocation;
	mAllRenderItems.push_back(std::move(gridRenderItem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylinderRenderItem = std::make_unique<RenderItem>();
		auto rightCylinderRenderItem = std::make_unique<RenderItem>();
		auto leftSphereRenderItem = std::make_unique<RenderItem>();
		auto rightSphereRenderItem = std::make_unique<RenderItem>();

		XMMATRIX leftCylinderWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylinderWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 0.5f);
		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylinderRenderItem->World, leftCylinderWorld);
		leftCylinderRenderItem->ObjCBIndex = objCBIndex++;
		leftCylinderRenderItem->Geo = mGeometries["shapeGeo"].get();
		leftCylinderRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylinderRenderItem->IndexCount = leftCylinderRenderItem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylinderRenderItem->StartIndexLocation = leftCylinderRenderItem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylinderRenderItem->BaseVertexLocation = leftCylinderRenderItem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylinderRenderItem->World, rightCylinderWorld);
		rightCylinderRenderItem->ObjCBIndex = objCBIndex++;
		rightCylinderRenderItem->Geo = mGeometries["shapeGeo"].get();
		rightCylinderRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylinderRenderItem->IndexCount = rightCylinderRenderItem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylinderRenderItem->StartIndexLocation = rightCylinderRenderItem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylinderRenderItem->BaseVertexLocation = rightCylinderRenderItem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRenderItem->World, leftSphereWorld);
		leftSphereRenderItem->ObjCBIndex = objCBIndex++;
		leftSphereRenderItem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRenderItem->IndexCount = leftSphereRenderItem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRenderItem->StartIndexLocation = leftSphereRenderItem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRenderItem->BaseVertexLocation = leftCylinderRenderItem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRenderItem->World, rightSphereWorld);
		rightSphereRenderItem->ObjCBIndex = objCBIndex++;
		rightSphereRenderItem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRenderItem->PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRenderItem->IndexCount = rightSphereRenderItem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRenderItem->StartIndexLocation = rightSphereRenderItem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRenderItem->BaseVertexLocation = rightSphereRenderItem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mAllRenderItems.push_back(std::move(leftCylinderRenderItem));
		mAllRenderItems.push_back(std::move(rightCylinderRenderItem));
		mAllRenderItems.push_back(std::move(leftSphereRenderItem));
		mAllRenderItems.push_back(std::move(rightSphereRenderItem));
	}

	for (auto& e : mAllRenderItems)
	{
		mOpaqueRenderItems.push_back(e.get());
	}
}

void ShapesApp::BuildDescriptorHeaps()
{
	UINT objCount = (UINT)mOpaqueRenderItems.size();
	UINT numDescriptors = (objCount + 1) * gNumFrameResources;
	mPassCBVOffset = objCount * gNumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap))
	);
}

void ShapesApp::BuildConstantBufferViews()
{
	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT objCount = (UINT)mOpaqueRenderItems.size();

	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto objectCB = mFrameResources[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();
			cbAddress += i * objCBByteSize;
			int heapIndex = frameIndex * objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = CalcConstantBufferByteSize(sizeof(PassConstants));

	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto passCB = mFrameResources[frameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		int heapIndex = mPassCBVOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
	}
}

void ShapesApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& items)
{
	UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));
	auto objectCB = mCurrentFrameResource->ObjectCB->Resource();

	for (std::size_t i = 0; i < items.size(); ++i)
	{
		auto renderItem = items[i];
		cmdList->IASetVertexBuffers(0, 1, &renderItem->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&renderItem->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(renderItem->PrimitiveTopology);

		UINT cbvIndex = mCurrentFrameResourceIndex * (UINT)mOpaqueRenderItems.size() + renderItem->ObjCBIndex;
		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart());
		handle.Offset(cbvIndex, mCbvSrvUavDescriptorSize);

		cmdList->SetGraphicsRootDescriptorTable(0, handle);
		cmdList->DrawIndexedInstanced(renderItem->IndexCount, 1, 
			renderItem->StartIndexLocation, renderItem->BaseVertexLocation, 0);
	}
}

void ShapesApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(
			md3dDevice.Get(), 1, (UINT)mAllRenderItems.size()
			));
	}
}

void ShapesApp::UpdateObjectCBs()
{
	auto currObjectCB = mCurrentFrameResource->ObjectCB.get();

	for (auto& e : mAllRenderItems)
	{
		if (e->NumFrameDirty > 0) 
		{
			XMMATRIX World = XMLoadFloat4x4(&e->World);
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(World));
			currObjectCB->CopyData(e->ObjCBIndex, objConstants);
			e->NumFrameDirty--;
		}
	}
}

void ShapesApp::UpdateMainPassCB()
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePos = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTagetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = mTimer.TotalTime();
	mMainPassCB.DeltaTime = mTimer.DeltaTime();

	auto currPassCB = mCurrentFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}
