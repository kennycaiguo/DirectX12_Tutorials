#pragma once
#include "d3dApp.h"
#include "FrameResource.h"

extern const int gNumFrameResources;

struct RenderItem 
{
	RenderItem() = default;
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	int NumFrameDirty = gNumFrameResources;
	UINT ObjCBIndex = -1;
	MeshGeometry* Geo = nullptr;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	UINT BaseVertexLocation = 0;
};

class ShapesApp : public D3DApp
{
public:
	using Super = D3DApp;
	ShapesApp(HINSTANCE hInstance) 
		: Super(hInstance) { }

protected:
	virtual void Update() override;
	virtual void Draw() override;

private:
	void BuildFrameResources();
	void UpdateObjectCBs();
	void UpdateMainPassCB();

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrentFrameResource = nullptr;
	int mCurrentFrameResourceIndex = 0;

	std::vector<std::unique_ptr<RenderItem>> mAllRenderItems;
	std::vector<RenderItem*> mOpaqueRenderItems;
	std::vector<RenderItem*> mTransparentRenderItems;

	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	PassConstants mMainPassCB;
	DirectX::XMFLOAT3 mEyePos;
};