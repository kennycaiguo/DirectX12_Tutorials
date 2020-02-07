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
	void BuildShapeGeometry();
	void BuildRenderItems();
	void BuildDescriptorHeaps();
	void BuildConstantBufferViews();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& items);
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

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCBVHeap;
	UINT mPassCBVOffset;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

	bool mIsWireframe;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
};